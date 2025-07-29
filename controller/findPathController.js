const { spawn } = require('child_process');

const findShortestPath = async(req, res) => {
    const { choice: algorithm, source, destination } = req.body;

    // Campus coordinates mapping
    const coordinates = {
        "main gate": [29.375000, 79.531111],
        "oat": [29.375278, 79.530000],
        "basketball": [29.375000, 79.530278],
        "cricket ground": [29.375000, 79.529444],
        "bus": [29.374444, 79.531111],
        "DS": [29.375278, 79.529722],
        "academic block a": [29.375278, 79.530833],
        "academic block c": [29.374444, 79.530556],
        "academic block d": [29.374444, 79.530000],
        "library": [29.375556, 79.530556],
        "canteen": [29.375000, 79.530833],
        "volleyball ground": [29.375000, 79.530000],
        "hostel": [29.375833, 79.529444],
        "saisandhya hall": [29.375556, 79.530000],
        "academic block b": [29.374722, 79.529722]
    };

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
                
                    osm_link = generateMapURL(jsonResult.path, coordinates);
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
                        osm_link = generateMapURL(pathString, coordinates);
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

// Fixed function to generate OpenStreetMap URL from path string
function generateMapURL(pathString, coordinates) {
    try {
        console.log('Generating map URL for path:', pathString); // Debug line
        
        // Parse the path string (e.g., "main gate → basketball")
        const locations = pathString.split(' → ').map(loc => loc.trim());
        console.log('Split locations:', locations); // Debug line
        
        // Build coordinate string for OpenStreetMap
        const coords = locations.map(location => {
            // Try exact match first, then lowercase match
            let coord = coordinates[location];
            if (!coord) {
                coord = coordinates[location.toLowerCase()];
            }
            
            console.log(`Location: "${location}" -> Coord:`, coord); // Debug line
            
            if (coord) {
                return `${coord[0]},${coord[1]}`;
            }
            return null;
        }).filter(coord => coord !== null);
        
        console.log('Final coords array:', coords); // Debug line
        
        if (coords.length > 0) {
            const baseURL = "https://www.openstreetmap.org/directions?engine=fossgis_osrm_foot&route=";
            const routeCoords = coords.join(';');
            const finalURL = baseURL + routeCoords;
            console.log('Final URL:', finalURL); // Debug line
            return finalURL;
        }
        
        console.log('No valid coordinates found'); // Debug line
        return null;
    } catch (error) {
        console.error('Error generating map URL:', error);
        return null;
    }
}

module.exports = { findShortestPath };