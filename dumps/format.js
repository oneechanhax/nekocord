
if (process.argv.length < 2)
	throw new Error('No file to process')

const fs = require('fs-extra')

var args = process.argv.slice(2);
for (var i = 0; i < args.length; i++) {
	var file = args[i]
	console.log("File: ", file)
	// Read it
	var data = fs.readFileSync(file)
	// Parse it
	var data = JSON.parse(data)
	// Write it
	fs.writeJsonSync(file, data, {spaces: 4, EOL: '\n', flag: 'w'})
}
