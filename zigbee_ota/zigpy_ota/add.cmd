echo [] > index.json
for %%a in (../*.zigbee) do (
start /wait node scripts/add.js ../%%a 
)
del *.zigbee
