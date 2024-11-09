# Image Processing Pipeline and Web Server

## Overview

This project consists of a suite of image filter programs and a web server developed in C. The image filters can be applied to bitmap images through a command-line interface or a web browser. The filters include copy, greyscale, Gaussian blur, edge detection, and scaling.

## Original Features

### Image Filter Programs
- **Filter Development & Process Management:** Engineered image filters in C, including copy, greyscale, Gaussian blur, edge detection, and scaling. Built a "master" program to manage the execution of multiple filter processes, enabling users to apply a series of filters to manipulate bitmap images in a single pipeline.

### Web Server
- **Web Server & HTTP Handling:** Developed a web server in C that utilizes sockets for network communication, enabling users to apply image filters via a web browser. Implemented HTTP request parsing and response construction to handle GET and POST requests, facilitating file uploads and filter application.

## How to Use

1. **Compile the Code:**
   Run `make` to compile the image filters and the web server.
   ```sh
   make
   ```

2. **Run Image Filters via Command Line:**
   Apply filters to an image using the command line. For example, to apply the greyscale filter:
   ```sh
   ./greyscale < input_image.bmp > output_image.bmp
   ```

3. **Run the Web Server:**
   Start the web server.
   ```sh
   ./image_server
   ```

4. **Access the Web Interface:**
   Open a web browser and navigate to `http://localhost:<port>/main.html` (replace `<port>` with the port number specified in the Makefile).

5. **Apply Filters via Web Interface:**
   Use the web interface to upload images and apply filters.

## Directory Structure

- **filters/**: Contains the compiled image filter executables.
- **images/**: Contains sample bitmap images for testing.
- **src/**: Contains the source code for the image filters and web server.
- **Makefile**: Instructions for compiling the project.

## Acknowledgements

Original author credit to David Liu for the project outline and initial code structure.
```

Feel free to adjust or expand upon this README as needed to better fit your project details.
