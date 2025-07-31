const { spawn } = require('child_process');

const findShortestPath = async(req, res) => {
    const { choice: algorithm, source, destination } = req.body;

    const child = spawn('integrated.exe', [algorithm, source, destination, '--web']);
    
    let output = '';
    let result = null;
    let osm_link = null;

    child.stdout.on('data', (data) => {
        output += data.toString();
    });

    child.stderr.on('data', (data) => {
        console.error('Error:', data.toString());
    });

    child.on('close', (code) => {
        if (code === 0) {
            console.log('Raw output:', output);
            
            // Try to parse as JSON first
            try {
                const jsonResult = JSON.parse(output.trim());
                console.log('Successfully parsed JSON:', jsonResult);
                
                if (jsonResult.success) {
                    result = `Algorithm: ${jsonResult.algorithm}
Distance: ${jsonResult.distance} meters
Time: ${jsonResult.time} minutes
Steps: ${jsonResult.steps}
Path: ${jsonResult.path}`;
                
                    osm_link = generateMapURL(jsonResult.path);
                } else {
                    result = `Error: ${jsonResult.error}`;
                }
            } catch (error) {
                console.log('Not JSON, parsing as text output');
                
                // Handle text output
                result = output;
                
                // Extract path from text (look for "Path: " line)
                const lines = output.split('\n');
                for (const line of lines) {
                    if (line.includes('Path:')) {
                        const pathString = line.replace('Path:', '').trim();
                        console.log('Extracted path:', pathString);
                        osm_link = generateMapURL(pathString);
                        break;
                    }
                }
            }
        } else {
            result = "Error calculating path: Process exited with code " + code;
        }

        res.render('Map', { result: result, osm_link: osm_link });
    });
};

function generateMapURL(pathString) {
    const coordinates = {
        "main gate": [29.375000, 79.531111],
        "oat": [29.375278, 79.530000],
        "basketball": [29.375000, 79.530278],
        "cricket ground": [29.375000, 79.529444],
        "bus": [29.374444, 79.531111],
        "ds": [29.375278, 79.529722],  // Note: your C code uses "DS" (uppercase)
        "academic block a": [29.375278, 79.530833],
        "academic block b": [29.374722, 79.529722],
        "academic block c": [29.374444, 79.530556],
        "academic block d": [29.374444, 79.530000],
        "library": [29.375556, 79.530556],
        "canteen": [29.375000, 79.530833],
        "volleyball ground": [29.375000, 79.530000],
        "hostel": [29.375833, 79.529444],
        "saisandhya hall": [29.375556, 79.530000]
    };

    try {
        console.log('Generating map URL for path:', pathString);
        
        let locations = [];
        
        // Handle ASCII arrow separator (recommended)
        if (pathString.includes(' -> ')) {
            locations = pathString.split(' -> ');
        }
        // Fallback for other separators
        else if (pathString.includes(' → ')) {
            locations = pathString.split(' → ');
        }
        else if (pathString.includes(' ? ')) {
            locations = pathString.split(' ? ');
        }
        else {
            // If no separator found, try to extract locations
            console.log('No standard separator found, attempting extraction...');
            return null;
        }
        
        // Clean and normalize location names
        locations = locations
            .map(loc => loc.trim().toLowerCase())
            .filter(loc => loc.length > 0);
        
        console.log('Split locations:', locations);
        
        // Build coordinate string for OpenStreetMap
        const coords = locations.map(location => {
            const coord = coordinates[location];
            console.log(`Location: "${location}" -> Coord:`, coord);
            
            if (coord) {
                return `${coord[0]},${coord[1]}`;
            }
            return null;
        }).filter(coord => coord !== null);
        
        console.log('Final coords array:', coords);
        
        if (coords.length > 0) {
            const baseURL = "https://www.openstreetmap.org/directions?engine=fossgis_osrm_foot&route=";
            const routeCoords = coords.join(';');
            const finalURL = baseURL + routeCoords;
            console.log('Final URL:', finalURL);
            return finalURL;
        }
        
        return null;
        
    } catch (error) {
        console.error('Error generating map URL:', error);
        return null;
    }
}

module.exports = { findShortestPath };