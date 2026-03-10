#include "Image.h"
#include "StickerSheet.h"

int main() {
  //
  // Reminder:
  //   Before exiting main, save your creation to disk as myImage.png
  //
  Image alma; alma.readFromFile("../data/alma.png");
  Image i;    i.readFromFile("../data/i.png");

  StickerSheet sheet(alma);
  sheet.addSticker(i, 800, 200);
  sheet.addSticker(i, 50, 500);

  sheet.removeSticker(1);

  const Image &renderXBound = sheet.render();

  //REQUIRE( renderXBound.width() == i.width() + 800 );
  //REQUIRE( renderXBound.height() == alma.height() );

  //checkStickerPlacement(i, renderXBound, 800, 200);

  sheet.removeSticker(0);

  //REQUIRE( sheet.render() == alma );

  sheet.addSticker(i, 20, 500);

  const Image &renderYBound = sheet.render();

  //REQUIRE( renderYBound.width() == alma.width() );
  //REQUIRE( renderYBound.height() == i.height() + 500 );

  //checkStickerPlacement(i, renderYBound, 20, 500);

  sheet.removeSticker(0);

  //REQUIRE( sheet.render() == alma );

  sheet.addSticker(i, 800, 200);
  sheet.addSticker(i, -100, -500);

  const Image &renderXYBound = sheet.render();

  //checkStickerPlacement(i, renderXYBound, 900, 700);
  //checkStickerPlacement(i, renderXYBound, 0, 0);

  sheet.removeSticker(1);
  sheet.removeSticker(0);

  sheet.render().writeToFile("expected_output_png_6.png");


  return 0;
}
