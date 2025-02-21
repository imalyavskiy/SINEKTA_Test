#include <cstdio>
int main()
{
/*
     Escapes
      \033   - octal
      \x1B   - hexademical
      \u001b - unicode
     
     '[' - Control Sequnce Introducer(CSI)
      <command>[;<command>[;<command>[...]]]
     ends with 'm'
*/
    printf("\033[31mHello world!\033[0m\n");
    printf("\x1B[36mHello world!\x1B[0m\n");
    printf("\u001b[35mHello world!\u001b[0m\n");
    printf("\n");

/*     
     Full example for foreground color change
     \x1B[36m 3 - type=foreground, 6 - color=red
      types:
          3  - foreground(text)
          4  - background
          9  - bright foreground(text)
          10 - bright background
          0  - reset formatting
      colors:
          0 - Black
          1 - Red
          2 - Green
          3 - Yellow
          4 - Blue
          5 - Magenta
          6 - Cyan
          7 - White
          9 - Default
          0 - Reser
*/          
    printf("This is \x1b[31mRED\x1b[0m text\n");
    printf("This is \x1b[41mRED\x1b[0m background\n");
    printf("This is \x1b[91mBRIGHT RED\x1b[0m text\n");
    printf("This is \x1b[101mBRIGHT RED\x1b[0m background\n");

    printf("\n");

    printf("This is \x1B[30;42mBlack on Normal Green\x1B[0m text\n");
    printf("This is \x1B[30;102mBlack on Bright Green\x1B[0m text\n");
    
    return 0;
}