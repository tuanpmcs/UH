/*
	This is a demo program designed to teach you how to compile an application that uses
	multiple libraries:

	1) OpenGL is a graphics application programming interface (API) designed to interact
		directly with graphics cards to draw three-dimensional stuff on the screen.
		https://www.opengl.org/

	2) GLFW is a "window manager" specifically designed for OpenGL. It works with the
		operating system to create a window that acts as an OpenGL "context", which is
		a part of the screen that acts as a target for OpenGL commands.
		https://www.glfw.org/

	3) ImGui is a real-time graphical user interface library. This uses a graphics API
		like OpenGL to render standard GUI objects like buttons, menus, list boxes, etc.
		The GitHub page is worth looking at: https://github.com/ocornut/imgui
		Here is a web-based demo that shows you the code needed to create any UI element:
		https://pthom.github.io/imgui_manual_online/manual/imgui_manual.html

	4) GLM is a light-weight linear algebra library that defines C/C++ objects useful for
		2D and 3D graphics.
		https://github.com/g-truc/glm
*/

// This header files contains declarations for functions and variables that are used across multiple C files.
#include "helloworld.h"

// I include this file to throw runtime errors, but you can also use it to output debugging information.
#include <iostream>

#include "ray.h"
#include "vec3.h"

#include <cmath>

int resolution = 500;					// resolution of the output image (you can add a user interface element to change this)
float* output_image_ptr = nullptr;		// pointer to the output image data (if you change the resolution make sure to change this!)
float frame_seconds = 0.0f;		// time it takes to go through the main "game" loop (directly translates to frame rate or fps)

// camera parameters
auto focal_length = 1.0;
auto viewport_height = 2.0;
auto viewport_width = 2.0;
point3 camera_center = point3(0, 0, 0);

// horizontal and vertical axes
vec3 viewport_u = vec3(viewport_width, 0, 0);
vec3 viewport_v = vec3(0, -viewport_height, 0);

// delta pixel size
auto pixel_delta_u = viewport_u / resolution;
auto pixel_delta_v = viewport_v / resolution;

// view port upper left corner
point3 viewport_upper_left = camera_center - viewport_u / 2 - viewport_v / 2 - vec3(0, 0, focal_length);

// Pixel 0, 0
auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

/*
 * Create a placeholder image that's simple, but looks interesting enough so that you know the code is working correctly.
 */
void DummyImage() {
	for (int yi = 0; yi < resolution; yi++) {							// iterate through each pixel in the image
		for (int xi = 0; xi < resolution; xi++) {
			float r = (float)xi / (float)resolution;					// calculate red, green, and blue values that depend on pixel position
			float g = (float)yi / (float)resolution;
			float b = (float)(resolution - xi) / (float)resolution;

			int idx = yi * resolution * 4 + xi * 4;						// calculate the starting position for the current pixel
			output_image_ptr[idx + 0] = r;								// update the red, green, blue, and alpha channels in the image
			output_image_ptr[idx + 1] = g;
			output_image_ptr[idx + 2] = b;
			output_image_ptr[idx + 3] = 1.0f;
		}
	}
}

double HitSphere(const point3& s, float r, const ray& rt) {
	auto p = rt.origin();
	auto v = rt.direction();

	auto a = dot(v, v);
	auto b = 2.0 * dot(v, s-p);
	auto c = dot(s - p, s - p) - r * r;

	auto h = b * b - 4 * a * c;

	if (h < 0)
	{
		return -1.0;
	}

	return (b - std::sqrt(h)) / (2.0 * a);
}

// r(t) = a + t*b
// s(t) = (s - a)(s - a) - r^2 = 0

color RayColor(const ray& r) {
	auto t = HitSphere(point3(0, 0, -1), 0.5, r);
	
	if (t > 0.0) {
		vec3 normal = unit_vector(r.at(t) - point3(0, 0, -1));
		return 0.5 * (normal + 1.0);
	}

	// BlendedValue = (1-a)*StartValue + a*EndValue
	auto unit_direction = unit_vector(r.direction());

	auto a = 0.5 * (unit_direction.y() + 1.0);

	auto start_color = color(1.0, 1.0, 1.0);		// white
	auto end_color = color(0.5, 0.7, 1.0);		// light blue

	return (1.0 - a) * start_color + a * end_color;
}

void DrawSquare() {
	for (int yi = 0; yi < resolution; yi++) {							// iterate through each pixel in the image
		for (int xi = 0; xi < resolution; xi++) {
			auto pixel_center = pixel00_loc + xi * pixel_delta_u + yi * pixel_delta_v;
			auto ray_direction = pixel_center - camera_center;
			ray r = ray(camera_center, ray_direction);

			auto pixel = RayColor(r);

			int idx = yi * resolution * 4 + xi * 4;						// calculate the starting position for the current pixel
			output_image_ptr[idx + 0] = pixel.x();						// update the red
			output_image_ptr[idx + 1] = pixel.y();
			output_image_ptr[idx + 2] = pixel.z();
			output_image_ptr[idx + 3] = 1.0f;
		}
	}
}

void DrawSphere() {
	for (int yi = 0; yi < resolution; yi++) {							// iterate through each pixel in the image
		for (int xi = 0; xi < resolution; xi++) {

		}
	}
}

int main(int argc, const char* argv[]) {

	/*
	* GLFW is the window manager that we will be using. Its job is to create
	* an "OpenGL context", which is a region of the screen that is used as
	* a target for OpenGL commands. This function initializes the GLFW library
	* and throws an exception if there are any hardware or software issues that
	* prevent it from initializing.
	*/
	if (!glfwInit())
		throw std::runtime_error("Failed to initialize GLFW");

	/*
	* This function call uses GLFW to create a window, including an OpenGL context. The
	* GLFWwindow pointer will be used in any GLFW library calls that effect this particular
	* window. For example, if you want to assign a callback function that responds to keyboard
	* or mouse input, you'll pass the GLFWwindow pointer. This also allows you to generate
	* multiple windows - you just make another call to glfwCreateWindow() and store the window
	* in a different GLFWwindow pointer! The next line of code makes sure the window is
	* successfully created (and throws a runtime exception if something went wrong).
	*/
	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Hello World", nullptr, nullptr);
	if (window == nullptr)
		throw std::runtime_error("Failed to create GLFW window");

	/*
	* This function makes the OpenGL context assigned to the window we created the "current"
	* context. That means that any OpenGL calls that we make will be sent to this context. If
	* we had multiple windows, we would have to set the contexts to the appropriate window
	* before we started sending commands to draw stuff.
	*/
	glfwMakeContextCurrent(window);

	/*
	* Now we're going to initialize the ImGui library. This is a really cool library that
	* uses OpenGL (or any graphics API) to render standard graphical user interface components.
	*/
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// This sets the GUI style to one that I personally think looks cool
	ImGui::StyleColorsDark();

	/*
	* These functions tell ImGui to use OpenGL and GLFW to render the user interface
	* and also specifies the version of the OpenGL shading language (GLSL) to use
	* for rendering. We will discuss shading languages near the end of the class since this
	* is the foundation for GPU computing. For now, all you need to know is that it's a
	* sub-language that is part of OpenGL and used to render stuff to individual pixels in the window.
	*/	
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 130";
	ImGui_ImplOpenGL3_Init(glsl_version);

	/*
	* This initializes a library called the OpenGL Extension Wrangler (GLEW) library. This is mostly
	* used for Windows and is a side effect of how the OpenGL library works with the operating system.
	* Basically, by default Windows only provides access to a very early version of OpenGL. This library
	* "mines" the OpenGL libraries to figure out what extensions are ACTUALLY supported by your graphics
	* driver. Don't worry too much about this - you'll probably never use anything beyond these two lines
	* of code.
	*/
	if (glewInit() != GLEW_OK)
		throw std::runtime_error("Failed to initialize GLEW");


	/*
	 * Allocate space on the heap to store the image that will be displayed on the screen.
	 */
	output_image_ptr = new float[resolution * resolution * 4];

	/*
	 * This function creates a placeholder image so that you see a result on the screen the first time you run it.
	 * You should see an "RGB square" where the red and blue channels change along the x-axis and the green channel
	 * changes along the y-axis.
	 */
	// DummyImage();

	DrawSquare();


	/*
	* This is what we call the "main rendering loop" (sometimes in gaming you'll call it the "main game loop".
	* This is where everything in the program happens. Every iteration through the loop re-draws everything
	* onto the window (including the user interface). It also queries sources for user input (mouse,
	* keyboard, etc.) and updates variables based on callback functions that you define. This loop runs
	* continuously until you kill the program (ex. by pressing the X button to close the window).
	*/
	while (!glfwWindowShouldClose(window)) {
		auto start = std::chrono::high_resolution_clock::now();

		// This function checks potential input sources (keyboard, mouse, etc.) and executes callback functions
		glfwPollEvents();

		// This function tells OpenGL to clear the window (in this case it writes the color "black" to all pixels)
		glClear(GL_COLOR_BUFFER_BIT);

		// This function is defined in the helloworld_gui.cpp file and renders the user interface
		ImGuiRender();

		/*
		* This function takes advantage of a concept called "double buffering". There is a region
		* of memory on your graphics card that displays the contents of the screen. The monitor reads this region
		* of memory every time it refreshes. This introduces a problem: if you draw to this area of memory
		* the user may be able to see the individual objects that you draw, resulting in "flickering". Double
		* buffering prevents this by sending the draw calls to a "back buffer" that isn't displayed on the
		* computer monitor. This function switches the memory pointers so that the information on the monitor
		* is updated immediately.
		*/
		glfwSwapBuffers(window);                    // swap the double buffer

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> duration = end - start;
		frame_seconds = duration.count();
	}

	/*
	* All of these functions just destroy the stuff that we've created to make sure that there aren't any
	* memory leaks.
	*/
	ImGui_ImplOpenGL3_Shutdown();					// Shut down ImGui's connection with OpenGL
	ImGui_ImplGlfw_Shutdown();						// Shut down ImGui's connection with GLFW
	ImGui::DestroyContext();                        // Clear the ImGui user interface
	glfwDestroyWindow(window);                      // Destroy the GLFW rendering window
	glfwTerminate();                                // Terminate GLFW
}