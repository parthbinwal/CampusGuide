const { spawn } = require('child_process');
const findShortestPath = async(req, res) => {
    const { choice: algorithm, source, destination } = req.body;

    const child = spawn('integrated.exe', [algorithm, source, destination]);
    
    let output = '';
    let osm_link = null;

    child.stdout.on('data', (data) => {
        output += data.toString();
    });

    child.stderr.on('data', (data) => {
        console.error('Error:', data.toString());
    });

    child.on('close', (code) => {
        if (code === 0) {
            // Extract the OpenStreetMap URL (line starting with http)
            const lines = output.split('\n');
            for (const line of lines) {
                if (line.startsWith('http')) {
                    osm_link = line.trim();
                    break;
                }
            }
        } else {
            output = "Error calculating path: Process exited with code " + code;
        }

        res.render('map', { result: output, osm_link: osm_link });
    });
};
module.exports = { findShortestPath };