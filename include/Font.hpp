#include <string>


/**
 *  Stores all resources required for loading and drawing fonts on-screen.
 */ 
class Font {
 public:

  /**
   *  Create a font object from an inputted font (default 128x font size)
   */
  Font(std::string font);

  /**
   *  Create a font object from an inputted font and char size
   */ 
  Font(std::string font, int char_size);
 private:
  // generates glfw texture atlas
};