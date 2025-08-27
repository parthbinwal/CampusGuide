## Project Overview

The Campus Navigation System is a comprehensive web application designed to help users find the shortest paths between different locations on a college campus. The system combines modern web technologies with
classic pathfinding algorithms to provide real-time route calculation and interactive map visualization.
![image alt](https://github.com/parthbinwal/CampusGuide/blob/728da1a29ffa049b192702c34552ed4ad3aa7fa7/Screenshot%20(1).png)
![image alt](https://github.com/parthbinwal/CampusGuide/blob/841dc6114f3a2c3685fe1af677d7fb2d4381898d/screenshotcrop.png)



### Key Capabilities
- **Multi-Algorithm Pathfinding**: Supports Dijkstra, A*, and Floyd-Warshall algorithms
- **Real-time Path Calculation**: Uses a C executable for high-performance computing
- **Interactive Web Interface**: User-friendly forms with responsive design
- **Map Integration**: OpenStreetMap visualization with walking directions
- **User Management**: Secure authentication and session handling
- **Campus-specific**: Pre-configured with 15 campus locations

## Features

### Core Navigation Features
- **Algorithm Selection**: Choose between Dijkstra, A*, Floyd-Warshall, or comparison mode
- **Location Database**: 15 pre-configured campus locations with GPS coordinates
- **Distance Calculation**: Uses Vincenty formula for precise geographic distances
- **Walking Metrics**: Calculates time (minutes) and steps for walking routes
- **Path Visualization**: Generates OpenStreetMap URLs for interactive route viewing

### Web Application Features
- **User Authentication**: Registration, login, logout with bcrypt password hashing
- **Session Management**: Secure session-based authentication
- **Responsive Design**: Mobile-friendly interface with modern CSS
- **Error Handling**: Comprehensive error management and user feedback
- **Real-time Processing**: Spawns C executable for path calculations

### Technical Features
- **Cross-platform**: Windows-compatible C executable integration
- **JSON/Text Output**: Flexible data format handling from C executable
- **Memory Efficient**: Optimized algorithms for performance
- **Scalable Architecture**: Modular design for easy extensions

## Technology Stack

### Backend Technologies
- **Node.js**: JavaScript runtime environment
- **Express.js**: Web application framework
- **MongoDB**: NoSQL database for user data
- **Mongoose**: Object Data Modeling (ODM) library
- **bcrypt**: Password hashing and security
- **express-session**: Session management middleware

### Frontend Technologies
- **EJS**: Embedded JavaScript templating engine
- **CSS3**: Modern styling with animations and responsive design
- **HTML5**: Semantic markup and form validation
- **Font Awesome**: Icon library for UI elements

### External Components
- **C Executable**: [`integrated.exe`](integrated.exe) for pathfinding algorithms
- **OpenStreetMap**: Map visualization and routing service
- **FOSSGIS OSRM**: Pedestrian routing engine

### Development Tools
- **nodemon**: Development server with auto-restart
- **dotenv**: Environment variable management

## Project Architecture

### MVC Pattern Implementation
The project follows the Model-View-Controller (MVC) architectural pattern:

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│     Views       │    │   Controllers   │    │     Models      │
│   (EJS Files)   │◄──►│  (Auth & Path)  │◄──►│  (User Schema)  │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         └───────────────────────┼───────────────────────┘
                                 ▼
                    ┌─────────────────────────┐
                    │     Database Layer      │
                    │     (MongoDB)           │
                    └─────────────────────────┘
```

### Data Flow Architecture
```
User Request → Express Router → Controller → C Executable → Result Processing → View Rendering → Response
```

## Installation & Setup

### Prerequisites
- **Node.js** (v14.0 or higher)
- **MongoDB** (local installation or cloud service)
- **Windows OS** (for integrated.exe compatibility)
- **Git** (for version control)

### Step-by-Step Installation

1. **Clone the Repository**
   ```bash
   git clone <repository-url>
   cd Guide
   ```

2. **Install Dependencies**
   ```bash
   npm install
   ```

3. **Environment Configuration**
   Create a [`.env`](.env) file in the root directory:
   ```env
   PORT=8080
   MONGO_URI=mongodb://localhost:27017/guide
   ```

4. **Database Setup**
   - Install and start MongoDB
   - The application will automatically create the 'guide' database

5. **Verify C Executable**
   Ensure [`integrated.exe`](integrated.exe) is present and executable:
   ```bash
   integrated.exe 1 "main gate" "library" --web
   ```

6. **Start the Application**
   ```bash
   # Production mode
   npm start
   
   # Development mode with auto-reload
   npm run dev
   ```

7. **Access the Application**
   Open your browser and navigate to `http://localhost:8080`

## API Documentation

### Authentication Endpoints

#### GET `/`
- **Description**: Homepage with welcome message and navigation
- **Response**: Renders [`homepage.ejs`](views/homepage.ejs)
- **Public**: Yes

#### GET `/api/login`
- **Description**: Display login form
- **Controller**: [`authController.getLogin`](controller/authController.js)
- **Response**: Renders [`login.ejs`](views/login.ejs)
- **Public**: Yes

#### POST `/api/login`
- **Description**: Authenticate user credentials
- **Controller**: [`authController.postLogin`](controller/authController.js)
- **Request Body**:
  ```json
  {
    "email": "user@example.com",
    "password": "userpassword"
  }
  ```
- **Success**: Redirect to `/api/map`
- **Error**: HTTP 400 with error message

#### GET `/api/register`
- **Description**: Display registration form
- **Controller**: [`authController.getRegister`](controller/authController.js)
- **Response**: Renders [`register.ejs`](views/register.ejs)
- **Public**: Yes

#### POST `/api/register`
- **Description**: Create new user account
- **Controller**: [`authController.postRegister`](controller/authController.js)
- **Request Body**:
  ```json
  {
    "username": "johndoe",
    "email": "john@example.com",
    "password": "securepassword"
  }
  ```
- **Success**: Redirect to `/api/login`
- **Error**: HTTP 400/500 with error message

#### GET `/api/logout`
- **Description**: Destroy user session and logout
- **Controller**: [`authController.logout`](controller/authController.js)
- **Authentication**: Required
- **Response**: Redirect to `/api/login`

#### GET `/api/map`
- **Description**: Display navigation interface
- **Controller**: [`authController.getMap`](controller/authController.js)
- **Authentication**: Required (session-based)
- **Response**: Renders [`Map.ejs`](views/Map.ejs)

### Pathfinding Endpoints

#### POST `/find_path`
- **Description**: Calculate shortest path between locations
- **Controller**: [`findPathController.findShortestPath`](controller/findPathController.js)
- **Authentication**: Required
- **Request Body**:
  ```json
  {
    "choice": "1",              // Algorithm: 1=Dijkstra, 2=A*, 3=Floyd-Warshall
    "source": "main gate",      // Source location
    "destination": "library"    // Destination location
  }
  ```
- **Process Flow**:
  1. Spawns [`integrated.exe`](integrated.exe) with parameters
  2. Processes JSON/text output from C executable
  3. Generates OpenStreetMap URL using [`generateMapURL`](controller/findPathController.js)
  4. Renders results in [`Map.ejs`](views/Map.ejs)

- **Success Response**: Renders map with results and OSM link
- **Error Response**: Renders map with error message

## Database Schema

### User Model ([`models/userMode.js`](models/userMode.js))

```javascript
{
  username: {
    type: String,
    required: true,
    description: "User's display name"
  },
  email: {
    type: String,
    required: true,
    unique: true,
    description: "User's email address (login identifier)"
  },
  password: {
    type: String,
    required: true,
    description: "Hashed password using bcrypt"
  }
}
```

### Campus Locations Database

The system uses a hardcoded location database in both the C code and Node.js controller:

| Location Name | Coordinates | Type | Description |
|---------------|-------------|------|-------------|
| main gate | [29.375000, 79.531111] | Entrance | Primary campus entrance |
| oat | [29.375278, 79.530000] | Entertainment | Open Air Theatre |
| basketball | [29.375000, 79.530278] | Sports | Basketball court |
| cricket ground | [29.375000, 79.529444] | Sports | Cricket playing field |
| bus | [29.374444, 79.531111] | Transport | Bus stop |
| DS | [29.375278, 79.529722] | Academic | Department building |
| academic block a | [29.375278, 79.530833] | Academic | Academic building A |
| academic block b | [29.374722, 79.529722] | Academic | Academic building B |
| academic block c | [29.374444, 79.530556] | Academic | Academic building C |
| academic block d | [29.374444, 79.530000] | Academic | Academic building D |
| library | [29.375556, 79.530556] | Academic | Campus library |
| canteen | [29.375000, 79.530833] | Dining | Food court |
| volleyball ground | [29.375000, 79.530000] | Sports | Volleyball court |
| hostel | [29.375833, 79.529444] | Residential | Student accommodation |
| saisandhya hall | [29.375556, 79.530000] | Event | Multipurpose hall |

### Database Connection ([`database/dbConnection.js`](database/dbConnection.js))

```javascript
const connectDB = async () => {
  try {
    await mongoose.connect(process.env.MONGO_URI);
    console.log("database connected");
  } catch (error) {
    console.log('error');
    process.exit(1);
  }
}
```

## Algorithms

The system implements three classic pathfinding algorithms via [`integrated.c`](integrated.c):

### 1. Dijkstra's Algorithm
- **Choice Value**: 1
- **Type**: Single-source shortest path
- **Time Complexity**: O(V² + E)
- **Space Complexity**: O(V)
- **Best For**: Finding shortest paths from one source to all destinations
- **Implementation**: Uses priority queue with distance array
- **Output**: Complete path with distance and walking time

### 2. A* Algorithm
- **Choice Value**: 2
- **Type**: Heuristic-based pathfinding
- **Time Complexity**: O(b^d) where b=branching factor, d=depth
- **Space Complexity**: O(V)
- **Best For**: Goal-directed search with geographic heuristics
- **Heuristic Function**: Uses Vincenty formula for straight-line distance
- **Implementation**: Priority queue with f-score (g + h)
- **Advantage**: Often faster than Dijkstra for single destination

### 3. Floyd-Warshall Algorithm
- **Choice Value**: 3
- **Type**: All-pairs shortest paths
- **Time Complexity**: O(V³)
- **Space Complexity**: O(V²)
- **Best For**: Pre-computing distances between all location pairs
- **Implementation**: Dynamic programming with distance matrix
- **Output**: Can find shortest path between any two nodes

### Distance Calculation Methods

#### Vincenty Formula Implementation
```c
double vincenty(Node a, Node b) {
  // Accounts for Earth's ellipsoidal shape
  // More accurate than haversine formula
  // Iterative solution for inverse geodesic problem
}
```

#### Walking Metrics Calculation
- **Walking Speed**: 1.4 m/s (average human pace)
- **Step Length**: 0.8 m (average stride)
- **Time Formula**: `distance / walking_speed / 60` (minutes)
- **Steps Formula**: `distance / step_length`

### Algorithm Comparison Mode
- **Choice Value**: 4
- **Function**: Runs all algorithms and compares performance
- **Metrics**: Execution time, memory usage, path optimality
- **Use Case**: Academic analysis and algorithm evaluation

## Usage Guide

### For End Users

#### 1. Getting Started
1. **Registration**:
   - Navigate to the homepage
   - Click "Sign Up" to create an account
   - Fill in username, email, and password
   - Submit the registration form

2. **Login**:
   - Click "Login" from the homepage
   - Enter your email and password
   - You'll be redirected to the navigation interface

#### 2. Finding Routes
1. **Algorithm Selection**:
   - Choose from dropdown: Dijkstra (reliable), A* (fast), Floyd-Warshall (comprehensive)
   - Each algorithm has different performance characteristics

2. **Location Selection**:
   - Select source location from dropdown
   - Select destination location from dropdown
   - Both dropdowns contain all 15 campus locations

3. **Route Calculation**:
   - Click "Get Shortest Path"
   - Wait for processing (typically 1-3 seconds)
   - View results including distance, time, and steps

4. **Map Visualization**:
   - Click the OpenStreetMap link to view the route
   - Interactive map shows walking directions
   - Route optimized for pedestrian travel

#### 3. Understanding Results
- **Algorithm**: Which pathfinding method was used
- **Distance**: Total walking distance in meters
- **Time**: Estimated walking time in minutes
- **Steps**: Approximate number of walking steps
- **Path**: Complete route with waypoints

### For Developers

#### Adding New Campus Locations

1. **Update C Code** ([`integrated.c`](integrated.c)):
   ```c
   Node nodes[V] = {
       // ... existing locations ...
       {"new location", "new location", latitude, longitude}
   };
   ```

2. **Update Node.js Controller** ([`controller/findPathController.js`](controller/findPathController.js)):
   ```javascript
   const coordinates = {
       // ... existing coordinates ...
       "new location": [latitude, longitude]
   };
   ```

3. **Update Frontend Form** ([`views/Map.ejs`](views/Map.ejs)):
   ```html
   <option value="new location">New Location</option>
   ```

4. **Recompile C Executable**:
   ```bash
   gcc -o integrated.exe integrated.c -lm
   ```

#### Extending Algorithms
1. Add new algorithm implementation in [`integrated.c`](integrated.c)
2. Update choice handling in main function
3. Add new option to frontend dropdown
4. Test with various location combinations

#### Customizing UI
- Modify [`public/css/style.css`](public/css/style.css) for styling
- Update [`views/Map.ejs`](views/Map.ejs) for form layout
- Customize colors, fonts, and animations

## Configuration

### Environment Variables ([`.env`](.env))

```env
# Server Configuration
PORT=8080                                    # Application port (default: 3000)

# Database Configuration
MONGO_URI=mongodb://localhost:27017/guide    # MongoDB connection string
```

### Session Configuration ([`entry.js`](entry.js))

```javascript
app.use(session({
  secret: 'your-secret-key',    // Change for production
  resave: false,                // Don't save session if unmodified
  saveUninitialized: true,      // Save new sessions
  cookie: {
    secure: false               // Set to true for HTTPS in production
  }
}));
```

### Security Best Practices

#### Password Security
- **Hashing**: bcrypt with 10 salt rounds
- **Validation**: Server-side input validation
- **Session**: Secure session management

#### Input Validation
- **Location Names**: Case-insensitive matching
- **Algorithm Choice**: Integer validation (1-4)
- **SQL Injection**: MongoDB ODM protection

#### Error Handling
- **User Errors**: Friendly error messages
- **System Errors**: Detailed logging without exposure
- **Path Not Found**: Graceful fallback handling

## File Structure

```
Guide/                              # Root directory
├── .env                           # Environment variables
├── .gitignore                     # Git ignore rules
├── entry.js                       # Application entry point
├── integrated.c                   # C source code for algorithms
├── integrated.exe                 # Compiled pathfinding executable
├── package.json                   # Project dependencies and scripts
│
├── controller/                    # Business logic controllers
│   ├── authController.js         # Authentication logic
│   └── findPathController.js     # Pathfinding logic
│
├── database/                      # Database connection
│   └── dbConnection.js           # MongoDB connection setup
│
├── middleware/                    # Custom middleware (currently empty)
│
├── models/                        # Database schemas
│   └── userMode.js               # User model definition
│
├── public/                        # Static assets
│   └── css/
│       └── style.css             # Application styles
│
├── router/                        # Route definitions
│   └── authRoutes.js             # Authentication routes
│
└── views/                         # EJS templates
    ├── homepage.ejs              # Landing page
    ├── login.ejs                 # Login form
    ├── Map.ejs                   # Main navigation interface
    └── register.ejs              # Registration form
```

### Key Files Description

#### Core Application Files
- [`entry.js`](entry.js): Express server setup, middleware configuration, route mounting
- [`package.json`](package.json): Dependencies, scripts, project metadata
- [`.env`](.env): Environment-specific configuration
- [`integrated.exe`](integrated.exe): Compiled C executable for pathfinding

#### Controllers
- [`authController.js`](controller/authController.js): User authentication, session management
- [`findPathController.js`](controller/findPathController.js): Path calculation, C executable integration

#### Models & Database
- [`userMode.js`](models/userMode.js): User schema with Mongoose
- [`dbConnection.js`](database/dbConnection.js): MongoDB connection logic

#### Views & UI
- [`homepage.ejs`](views/homepage.ejs): Welcome page with navigation
- [`login.ejs`](views/login.ejs): User login form
- [`register.ejs`](views/register.ejs): User registration form
- [`Map.ejs`](views/Map.ejs): Main navigation interface with dropdowns
- [`style.css`](public/css/style.css): Complete styling with responsive design

## Troubleshooting

### Common Issues and Solutions

#### 1. "integrated.exe not found" Error
**Problem**: C executable cannot be located or executed

**Solutions**:
- Verify [`integrated.exe`](integrated.exe) is in the project root directory
- Check file permissions (ensure executable rights)
- Recompile from source if needed:
  ```bash
  gcc -o integrated.exe integrated.c -lm
  ```
- Ensure Windows compatibility (exe files are Windows-specific)

#### 2. Database Connection Failed
**Problem**: Cannot connect to MongoDB

**Solutions**:
- Start MongoDB service:
  ```bash
  # Windows
  net start MongoDB
  
  # Linux/Mac
  sudo systemctl start mongod
  ```
- Verify connection string in [`.env`](.env)
- Check MongoDB is running on correct port (27017)
- Ensure database permissions are correct

#### 3. Session/Authentication Issues
**Problem**: Users can't login or sessions expire unexpectedly

**Solutions**:
- Clear browser cookies and cache
- Check session configuration in [`entry.js`](entry.js)
- Verify secret key is set properly
- Ensure MongoDB is storing session data correctly

#### 4. Path Calculation Errors
**Problem**: "No valid path found" or calculation timeouts

**Solutions**:
- Verify location names match exactly (case-sensitive in C code)
- Check [`integrated.c`](integrated.c) location array
- Ensure source and destination are different
- Review C executable output in console logs

#### 5. OpenStreetMap Links Not Working
**Problem**: Generated map URLs are invalid or don't display routes

**Solutions**:
- Verify coordinates in [`findPathController.js`](controller/findPathController.js)
- Check path string parsing logic
- Ensure separators match C executable output format
- Test OpenStreetMap service availability

#### 6. CSS/Styling Issues
**Problem**: Layout broken or styles not loading

**Solutions**:
- Verify static files middleware in [`entry.js`](entry.js)
- Check [`style.css`](public/css/style.css) path
- Clear browser cache
- Ensure CSS file has proper permissions

#### 7. Performance Issues
**Problem**: Slow response times or memory usage

**Solutions**:
- Optimize C executable compilation with flags:
  ```bash
  gcc -O2 -o integrated.exe integrated.c -lm
  ```
- Implement caching for frequent route requests
- Monitor MongoDB query performance
- Use connection pooling for database

### Debug Mode Activation

Enable detailed logging for troubleshooting:

```javascript
// Add to entry.js
app.use((req, res, next) => {
  console.log(`${new Date().toISOString()} - ${req.method} ${req.url}`);
  next();
});

// Add to findPathController.js
console.log('Algorithm:', algorithm);
console.log('Source:', source);
console.log('Destination:', destination);
console.log('C executable output:', output);
```

### System Requirements

#### Minimum Requirements
- **RAM**: 2GB
- **Disk Space**: 500MB
- **CPU**: 1GHz processor
- **OS**: Windows 7/8/10/11
- **Node.js**: v14.0+
- **MongoDB**: v4.0+

#### Recommended Requirements
- **RAM**: 4GB or more
- **Disk Space**: 1GB
- **CPU**: Multi-core processor
- **OS**: Windows 10/11
- **Node.js**: v16.0+
- **MongoDB**: v5.0+

### Performance Monitoring

#### Key Metrics to Monitor
1. **Response Time**: Path calculation duration
2. **Memory Usage**: C executable and Node.js process
3. **Database Queries**: MongoDB response times
4. **Concurrent Users**: Session management performance
5. **Error Rates**: Failed path calculations

#### Monitoring Tools
- Use `console.time()` and `console.timeEnd()` for timing
- Monitor MongoDB with built-in profiling
- Use Node.js built-in `process.memoryUsage()`
- Implement health check endpoints

## Contributing

### Development Workflow
1. Fork the repository
2. Create feature branch: `git checkout -b feature/new-feature`
3. Make changes and test thoroughly
4. Commit with descriptive messages
5. Push to your fork: `git push origin feature/new-feature`
6. Submit pull request with detailed description

### Code Standards
- **JavaScript**: Use ESLint with recommended settings
- **C Code**: Follow GNU coding standards
- **CSS**: Use BEM methodology for naming
- **Documentation**: Update README for any new features

### Testing Guidelines
- Test all authentication flows
- Verify path calculations with known distances
- Test responsive design on multiple devices
- Validate form inputs and error handling

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Support & Contact

For technical support:
- Check this documentation first
- Review GitHub issues for similar problems
- Create detailed bug reports with steps to reproduce
- Contact the development team for urgent issues

---

**Project Version**: 1.0.0  
**Last Updated**: [Current Date]  
**Maintained By**: Campus Navigation Team
