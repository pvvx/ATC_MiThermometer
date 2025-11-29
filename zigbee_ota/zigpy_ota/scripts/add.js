const path = require('path');
const fs = require('fs');
const crypto = require('crypto');
const tls = require('tls');
const ota = require('./ota');
const filenameOrURL = process.argv[2];
const baseURL = process.argv[3];
const caCerts = './cacerts.pem';

const manufacturerNameLookup = {
    123: 'UHome',
    4098: 'Tuya',
    4107: 'Hue',
    4117: 'Develco',
    4129: 'Legrand',
    4151: 'Jennic',
    4190: 'SchneiderElectric',
    4364: 'Osram',
    4405: 'DresdenElektronik',
    4417: 'Telink',
    4420: 'Lutron',
    4444: 'Danalock',
    4447: 'Lumi',
    4448: 'Sengled',
    4454: 'Innr',
    4474: 'Insta',
    4476: 'IKEA',
    4489: 'Ledvance',
    4617: 'Bosch',
    4644: 'Namron',
    4648: 'Terncy',
    4659: 'ThirdReality',
    4678: 'Danfoss',
    4687: 'Gledopto',
    4714: 'EcoDim',
    4742: 'Sonoff',
    4747: 'NodOn',
    4919: 'Datek',
    4877: 'thirdreality',
    4636: 'Aurora',
    5127: '3R',
    26214: 'Sprut.device',
    10132: 'ClimaxTechnology',
    60000: 'Telink-pvvx',
};

const main = async () => {
    if (!filenameOrURL) {
        throw new Error('Please provide a filename or URL');
    }

    const isURL = filenameOrURL.toLowerCase().startsWith("http");
    const files = [];

    if (isURL) {
        const downloadFile = async (url, path) => {
            const lib = url.toLowerCase().startsWith("https") ? require('https') : require('http');
            const file = fs.createWriteStream(path);

            return new Promise((resolve, reject) => {
                const ca = [...tls.rootCertificates];
                if(fs.existsSync(caCerts)) {
                    ca.push(fs.readFileSync(caCerts));
                }
                const request = lib.get(url, { ca },  function(response) {
                    if (response.statusCode >= 200 && response.statusCode < 300) {
                        response.pipe(file);
                        file.on('finish', function() {
                          file.close(function() {
                              resolve();
                          });
                        });
                    } else if (response.headers.location) {
                        resolve(downloadFile(response.headers.location, path));
                    } else {
                        reject(new Error(response.statusCode + ' ' + response.statusMessage));
                    }
                });
            });
        }

        const file = path.resolve("temp");
        await downloadFile(filenameOrURL, file);
        files.push(file);
    } else {
        const file = path.resolve(filenameOrURL);
        if (fs.lstatSync(file).isFile()) {
            if (!fs.existsSync(file)) {
                throw new Error(`${file} does not exist`);
            }
            files.push(file);
        } else {
            const otaExtension = ['.ota', '.zigbee'];
            const otasInDirectory = fs.readdirSync(file)
                .filter((f) => otaExtension.includes(path.extname(f).toLowerCase()))
                .map((f) => path.join(file, f));
            files.push(...otasInDirectory);
        }
    }

    for (const file of files) {
        const buffer = fs.readFileSync(file);
        const parsed = ota.parseImage(buffer);

        if (!manufacturerNameLookup[parsed.header.manufacturerCode]) {
            throw new Error(`${parsed.header.manufacturerCode} not in manufacturerNameLookup (please add it)`);
        }

        const manufacturerName = manufacturerNameLookup[parsed.header.manufacturerCode];
        const indexJSON = JSON.parse(fs.readFileSync('index.json'));
        //const destination = path.join('images', manufacturerName, path.basename(file));
        const destination = path.basename(file);

        const hash = crypto.createHash('sha512');
        hash.update(buffer);
        const hdString = parsed.header.otaHeaderString;
        const hdmanufacturer = [hdString.split(':')[0]];
        const modelId = hdString.split(':')[1];

        const entry = {
            fileVersion: parsed.header.fileVersion,
            fileSize: parsed.header.totalImageSize,
            manufacturerCode: parsed.header.manufacturerCode,
            imageType: parsed.header.imageType,
            sha512: hash.digest('hex'),
            otaHeaderString: hdString,
            //manufacturerName: manufacturerName,
        };

        if (modelId) {
            entry.modelId = modelId;
            entry.manufacturerName = hdmanufacturer;
        }

        if (isURL) {
            entry.url = filenameOrURL;
        } else {
            const destinationPosix = destination.replace(/\\/g, '/');
            entry.url = `${baseURL}/${escape(destinationPosix)}`;
            //entry.path = destinationPosix;
        }

        const index = indexJSON.findIndex((i) => {
            return i.manufacturerCode === entry.manufacturerCode && i.imageType === entry.imageType && (!i.modelId || i.modelId === entry.modelId)
        });

        if (index !== -1) {
            console.log(`Updated existing entry (${JSON.stringify(entry)})`);
            indexJSON[index] = {...indexJSON[index], ...entry};

            if (entry.path && entry.path !== destination) {
                try {
                    fs.unlinkSync(path.resolve(entry.path));
                } catch (err) {
                    if (err && err.code != 'ENOENT') {
                        console.error("Error in call to fs.unlink", err);
                        throw err;
                    }
                }
            }
        } else {
            console.log(`Added new entry (${JSON.stringify(entry)})`);
            indexJSON.push(entry);
        }

        if (!isURL && file !== path.resolve(destination)) {
            if (!fs.existsSync(path.dirname(destination))) {
                fs.mkdirSync(path.dirname(destination));
            }

            fs.copyFileSync(file, destination);
        }

        fs.writeFileSync('index.json', JSON.stringify(indexJSON, null, '    '));

        if (isURL) {
            fs.unlinkSync(file);
        }
    }
}

main();
