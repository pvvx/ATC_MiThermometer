echo [] > index.json
for %%a in (../bin/*.zigbee) do (
start /wait node scripts/add.js ../bin/%%a 
)
del *.zigbee
