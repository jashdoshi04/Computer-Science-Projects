/**
 * @file maptiles.cpp
 * Code for the maptiles function.
 */

#include <iostream>
#include <map>

#include "maptiles.h"

using namespace std;


Point<3> convertToXYZ(LUVAPixel pixel) {
    return Point<3>( pixel.l, pixel.u, pixel.v );
}

MosaicCanvas* mapTiles(SourceImage const& theSource,
                       vector<TileImage>& theTiles)
{
    /**
     * @todo Implement this function!
     */
    // returning null even though not needed just for good practice 
    if(theTiles.empty()){
        return NULL;
    }
    // create a new mosaic 
    MosaicCanvas *final_mosaic = new MosaicCanvas(theSource.getRows(),theSource.getColumns());

    // vector of converted points 
    vector <Point<3>> d3_points; 
    //make a map to store the average colours for each tile 
    map<Point<3>, TileImage* >source_colour_map;

    // fill up the map with the colours 
    for(auto& point:theTiles) {
        LUVAPixel avg = point.getAverageColor();
        // now convert it to 3 dimensional colour 
        Point<3> d3_colour = convertToXYZ(avg);
        d3_points.push_back(d3_colour);
        // add it to the map
        source_colour_map[d3_colour] = &point; 
    }
    
    
    // now create a KD tree for the converted points 
    KDTree<3> avg_colour_tree(d3_points);
    // now update tiles on the source image 
    for (int i = 0 ; i < theSource.getRows();i++) {
        for(int j= 0 ; j <theSource.getColumns();j++) {
            Point<3> tile = convertToXYZ(theSource.getRegionColor(i, j));
            //find the nearest point to tile we have 
            Point<3> closest = avg_colour_tree.findNearestNeighbor(tile);
            //set the tile  on the image 
            final_mosaic -> setTile(i,j,source_colour_map[closest]);
        }
    }
    return final_mosaic;
}

