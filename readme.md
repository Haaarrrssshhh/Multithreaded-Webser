# Harsh Zota

### 07700000128

# Distributed System Programming Assignment-1

## Project Summary
This project involves the development of a basic HTTP server in C for a distributed systems assignment. The server handles client connections, processes HTTP GET requests, and serves static files, including HTML pages (`index.html` and `info.html`), images, and videos. The server supports external video links and direct access to images or text files through URLs, while handling errors gracefully by serving specific error pages (`400.html`, `403.html`, `404.html`, and `500.html`). Logging features are implemented to track new connections, resource requests, permission issues, and connection closures. The project demonstrates fundamental concepts in socket programming, multithreading, and HTTP server functionality.

## Project Structure
![Alt text](/outputs/files.png)

## Instructions to Run the Program

1. **Setup the Environment**:
   - Ensure you have `gcc` installed on your system.
   - Navigate to the project directory containing the `src`, `webserver_files`, and other directories.

2. **Compile the Server Code**:
   ```bash
   make
   ```
   This will compile the source files and generate the server executable.

3. **Run the Server**:
   ```bash
   ./bin/server
   ```
   The server will start listening on the specified port (e.g., 8000 or 8888).

4. **Access the Server**:
   - Open your browser and visit `http://localhost:8888` to see the default page.
   - Test other paths like `info.html`, image files, or text files.

5. **Log Files**:
   - Logs are saved in `server.log` which captures connection events, file requests, errors, and other server-side activities.

## Images and Descriptions

### 1. Image of Logs
![Alt text](/outputs/logs1.png)
![Alt text](/outputs/logs2.png)
- This image shows the content of the `server.log` file, which records new connections, resource requests, and client disconnections, including detailed timestamps.

### 2. Image of Default Page
![Alt text](/outputs/default.png)
- The default page (`index.html`) is served when the root URL is accessed. It contains basic information about the project.

### 3. Image of `index.html` Page
![Alt text](/outputs/index.png)
- The `index.html` page served at the root directory is a simple HTML page, including links and text describing Santa Clara University.

### 4. Image of `info.html` Page
![Alt text](/outputs/pagewithimage.png)
- The `info.html` page provides information and examples of images being served by the server to test media requests.

### 5. Image of Directly Loading an Image
![Alt text](/outputs/directimage.png)
- This image shows the browser directly loading an image by accessing its specific path (`http://localhost:8000/assets/image.png`).

### 6. Image for Loading `sample.txt`
![Alt text](/outputs/sampletext.png)
- The `sample.txt` file is served directly, demonstrating how the server handles text files with the appropriate `Content-Type` header.

### 7. Image for Forbidden Access
![Alt text](/outputs/forbidden.png)
- This image shows the `403.html` page, which appears when trying to access a file with insufficient permissions.

### 8. Image of Invalid Path
![Alt text](/outputs/invalidpath.png)
- When a non-existent path is requested, the server returns a `500 Internal Server Error` page (`500.html`). This image shows the error page being served.

### 9. Image for Internal Server Error
![Alt text](/outputs/is1.png)
![Alt text](/outputs/is2.png)
- Similar to the previous error page, this demonstrates another case where the server returns an internal server error.

### 10. Image to Show Headers
![Alt text](/outputs/headers.png)
- This image is taken from browser developer tools, showcasing the HTTP headers sent by the server, including `Content-Type`, `Content-Length`, and connection management.

