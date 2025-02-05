const child_process = require('child_process');
const fs = require('fs');
const path = require('path');

const concatCaCerts = (folder = 'cacerts', outputFilename = 'cacerts.pem') => {
  const files = fs.readdirSync(folder);

  const caCertFiles = files.filter((file) => path.extname(file) === '.pem');
  const outputFile = fs.openSync(outputFilename, 'w');

  caCertFiles.forEach((caCert) => {
    const filePath = path.join(folder, caCert);
    const fileContent = fs.readFileSync(filePath, 'utf8');
    fs.appendFileSync(outputFile, fileContent + '\n');
  });
};

const main = async () => {
    concatCaCerts();
    const indexJSON = JSON.parse(fs.readFileSync('index.json'));
    indexJSON.forEach(entry => {
        const result = child_process.execSync(`node ./scripts/add.js "${entry.path || entry.url}" "${entry.modelId || ''}"`, {
            cwd: path.dirname(__dirname)
        })
        console.log(result.toString())
    })
}

return main();
