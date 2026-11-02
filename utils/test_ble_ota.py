"""SDK mode uses only the standard OTA service and checks the server result."""
import asyncio
import struct
import sys
import types
import unittest
from unittest.mock import patch
import ble_ota

class Scanner:
    @staticmethod
    async def find_device_by_filter(predicate, **kwargs):
        device=types.SimpleNamespace(address='MACOS-UUID')
        assert predicate(device,types.SimpleNamespace(rssi=-40))
        return device

class Client:
    result=0
    properties=["write"]
    instances=[]
    def __init__(self,device,**kwargs):
        self.services=types.SimpleNamespace(get_characteristic=lambda _:types.SimpleNamespace(properties=self.properties))
        self.writes=[];self.callback=None;self.disconnected=kwargs.get('disconnected_callback');self.instances.append(self)
    async def __aenter__(self):return self
    async def __aexit__(self,*args):pass
    async def start_notify(self,char,callback):
        assert char==ble_ota.OTA_CHAR, 'Generic SDK must not query thermometer CFG'
        self.callback=callback
    async def stop_notify(self,char):pass
    async def read_gatt_char(self,char):raise AssertionError('SDK result comes from notification, not thermometer status')
    async def write_gatt_char(self,char,data,**kwargs):
        assert char==ble_ota.OTA_CHAR
        if 'write' not in self.properties:assert not kwargs.get('response'), 'Write requests are not supported'
        self.writes.append(bytes(data))
        if data[:2]==b'\x02\xff':
            if self.result is not None:self.callback(char,bytes([6,255,self.result,0]))
            if self.disconnected:self.disconnected(self)

class TestSDK(unittest.IsolatedAsyncioTestCase):
    async def send(self,result):
        Client.result=result
        with patch.dict(sys.modules,{'bleak':types.SimpleNamespace(BleakClient=Client,BleakScanner=Scanner)}):
            return await ble_ota.update('MACOS-UUID',None,b'\x01'*20,False,True,sdk=True,result_timeout=.01)
    async def test_sdk_image_is_sent_and_device_validation_is_required(self):
        await self.send(0)
        writes=Client.instances[-1].writes
        self.assertIn(bytes.fromhex('01ff'),writes)
        self.assertEqual(writes[-1],bytes.fromhex('02ff0100feff'))
        self.assertEqual(writes[-2][2:18],b'\x01'*4+b'\xff'*12)
    async def test_write_without_response_only_characteristic(self):
        with patch.object(Client,'properties',['write-without-response']):
            await self.send(0)
    async def test_rejected_image_is_failure(self):
        with self.assertRaisesRegex(RuntimeError,'7'):await self.send(7)
    async def test_disconnect_without_device_result_is_not_success(self):
        with self.assertRaises((RuntimeError,TimeoutError)):await self.send(None)

class TestImage(unittest.TestCase):
    def test_explicit_sdk_slot_limit(self):
        fw=bytearray(193092);fw[8:12]=b'KNLT';struct.pack_into('<I',fw,24,len(fw))
        with self.assertRaises(SystemExit):ble_ota.check_image(fw)
        ble_ota.check_image(fw,max_size=208*1024)
        with self.assertRaises(SystemExit):ble_ota.check_image(fw,max_size=128*1024)
    def test_legacy_packet_index_never_collides_with_commands(self):
        with self.assertRaises(SystemExit):ble_ota.check_image(b'\0'*8+b'KNLT',max_size=0x100000)

if __name__=='__main__':unittest.main()
