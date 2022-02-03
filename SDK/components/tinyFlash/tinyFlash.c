#include "tl_common.h"
#include "drivers.h"

#include "tinyFlash.h"

#define TINY_SECTOR_SIZE 4096 //flash扇区大小
#define TINY_BUFFER_SIZE 256  //tiny缓冲区大小

#define TINY_SECHAD_SIZE 32   //记录扇区使用情况的扇区头大小

static unsigned char _buf[TINY_BUFFER_SIZE] = { 0 };

static unsigned long tinyFlash_Start_Addr = 0;
static unsigned long tinyFlash_End_Addr   = 0;

static unsigned long tinyFlash_Used_Addr = 0; //当前使用的扇区地址
static unsigned long tinyFlash_Swap_Addr = 0; //当前未使用的扇区地址

void flash_write(unsigned long addr, unsigned long len, unsigned char *buf)
{
    unsigned long tmp = addr & 0xff;

    if(tmp + len > 0x100) //跨扇区写入
    {
        flash_write_page(addr, 0x100 - tmp, buf);

        tmp = (0x100 - tmp);

        len -= tmp;

        buf += tmp;

        addr &= 0xffffff00;

        addr += 0x100;
    }

    flash_write_page(addr, len, buf);
}

// flash_erase_sector(addr);
// flash_write_page(addr, 256, buf);
// flash_read_page(unsigned long addr, unsigned long len, unsigned char *buf){

bool tinyFlash_Init(unsigned long start_addr, unsigned long len)
{
    tinyFlash_Start_Addr = start_addr;

    tinyFlash_End_Addr = start_addr + len;

    memset(_buf, 0 , TINY_BUFFER_SIZE);

    flash_read_page(tinyFlash_Start_Addr, TINY_BUFFER_SIZE, _buf);

    //at_print_hexstr(_buf, TINY_BUFFER_SIZE);

    if(_buf[0] != 0XFF)  //第一个扇区在使用
    {
        tinyFlash_Used_Addr = tinyFlash_Start_Addr;
        tinyFlash_Swap_Addr = tinyFlash_Start_Addr + TINY_SECTOR_SIZE;
    }
    else //第二个扇区在使用
    {
        tinyFlash_Used_Addr = tinyFlash_Start_Addr + TINY_SECTOR_SIZE;
        tinyFlash_Swap_Addr = tinyFlash_Start_Addr;
    }

    return true;
}
		
int tinyFlash_Read(unsigned char KEY, unsigned char * outbuf, unsigned char * len)
{
    unsigned long _addr_start = tinyFlash_Used_Addr + TINY_SECHAD_SIZE;;
    unsigned long _addr_end   = tinyFlash_Used_Addr + TINY_SECTOR_SIZE;
    
    while(1)
    {
        if(_addr_start > _addr_end -3) //该扇区查找完毕
        {
            break;
        }
        
        flash_read_page(_addr_start, TINY_BUFFER_SIZE, _buf);

        if(_buf[0] == KEY) //目标KEY
        {
            if(_buf[1] == (KEY ^ 0xFF)) //该KEY正在使用中
            {
                if(len == NULL)  //删除操作
                {
                    _buf[0] = 0;
                    flash_write_page(_addr_start + 1 , 1 , _buf);
                }
                else if(outbuf == NULL) //读取长度
                {
                    *len = _buf[2];
                }
                else //读取数据
                {
                    memcpy(outbuf, _buf + 3, _buf[2]);
                    *len = _buf[2];
                }

                return 0;
            }
            else //该KEY已被删除
            {
                //at_print_hexstr(_buf, 5);
                //at_print("delect\r\n");
                _addr_start += (_buf[2] + 3);
                continue;
            }
        }
        else if((_buf[0] != 0) && (_buf[0] != 0xff)) //其他KEY
        {
            _addr_start += (_buf[2] + 3);
            continue;
        }
        else //读取到扇区尾部未使用的区域
        {
            break;
        }
    }
    
    return -1;
}

unsigned long  g_addr = 0;

int tinyFlash_Write(unsigned char KEY, unsigned char * buf, unsigned char len)
{
    tinyFlash_Read(KEY,  NULL, NULL);

    unsigned long _addr_start = tinyFlash_Used_Addr + TINY_SECHAD_SIZE;;
    unsigned long _addr_end   = tinyFlash_Used_Addr + TINY_SECTOR_SIZE;

    unsigned long dirty_data_len = 0;
        
    while(1)
    {
        if(_addr_start > _addr_end -3 - len) //该扇区已查找完毕，无可用空间
        {
            if(dirty_data_len > 0) //当前扇区中存在脏数据
            {
                tinyFlash_Swap(); //交换新旧扇区，清理脏数据

                _addr_start = tinyFlash_Used_Addr + TINY_SECHAD_SIZE;;
                _addr_end   = tinyFlash_Used_Addr + TINY_SECTOR_SIZE;
            }
            else //无可用空间
            {
                return -1;
            }
            
            //at_print("Swap\r\n");
        }

        g_addr = _addr_start;
        
        flash_read_page(_addr_start, TINY_BUFFER_SIZE, _buf);

        if(_buf[0] == 0xFF) //该区域可使用
        {
            //at_print_hexstr(&_addr_start, 2);

            _buf[0] = KEY;
            _buf[1] = (KEY ^ 0xFF);
            _buf[2] = len;
            memcpy(_buf +3, buf, len);

            flash_write(_addr_start, len + 3, _buf);
            
            return 0;
        }
        else if(_buf[0] != 0) //已存储其他KEY
        {
            _addr_start += (_buf[2] + 3);
            
            if(_buf[1] != (KEY ^ 0xFF))
            {
                dirty_data_len += (_buf[2] + 3);
            }
            continue;
        }
        else //读取到错误的数据
        {
            // at_print_hexstr(_buf, 5);
            // at_print("Er\r\n");
            break;
        }
    }
    
    return 0;
}

void tinyFlash_Swap() //扇区使用完了，需要清理数据，才能存储别的数据
{
    unsigned long _addr_start = tinyFlash_Used_Addr + TINY_SECHAD_SIZE; //当前使用的扇区的起始地址
    unsigned long _addr_end   = tinyFlash_Used_Addr + TINY_SECTOR_SIZE; //当前使用的扇区的结束地址

    unsigned long _new_addr_start = tinyFlash_Swap_Addr + TINY_SECHAD_SIZE; //将要使用的扇区的起始地址
    //unsigned long _new_addr_end   = tinyFlash_Swap_Addr + TINY_SECTOR_SIZE; //将要使用的扇区的结束地址
        
    unsigned long tmp = 0;

    while(1)
    {
        if(_addr_start > _addr_end -3) //该扇区已查找完毕，无可用空间
        {
            break;
        }
        
        flash_read_page(_addr_start, TINY_BUFFER_SIZE, _buf);

        if(_buf[0] == 0xFF) //数据转移完毕
        {
            break;
        }
        else if(_buf[0] == 0) //读取到了错误的数据
        {
            break;
        }
        else //读取到正确的Key数据
        {
            _addr_start += (_buf[2] + 3);

            if(_buf[1] == (_buf[0] ^ 0xFF)) //数据仍有效(未删除)
            {
                tmp = _new_addr_start & 0xff;

                flash_write(_new_addr_start, _buf[2] + 3, _buf);
                _new_addr_start += (_buf[2] + 3);
            }
            continue;
        }
    }

    _buf[0] = 0xaa;
    flash_write_page(tinyFlash_Swap_Addr, 1, _buf); //将新扇区标记为在使用
    flash_erase_sector(tinyFlash_Used_Addr);        //擦除旧扇区

    _new_addr_start = tinyFlash_Swap_Addr;

    tinyFlash_Swap_Addr = tinyFlash_Used_Addr;
    tinyFlash_Used_Addr = _new_addr_start;
}

/*擦除所有扇区*/
void tinyFlash_Format(void)
{
    flash_erase_sector(tinyFlash_Used_Addr);//擦除旧扇区
    flash_erase_sector(tinyFlash_Swap_Addr);//擦除旧扇区
} 

/*读取某片区域的数据，存放到全局变量，主要用于Debug*/
void tinyFlash_Debug(unsigned long addr)
{
    flash_read_page(addr, TINY_BUFFER_SIZE, _buf);
} 