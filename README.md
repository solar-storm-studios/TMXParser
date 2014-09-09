**NOTE:**
TMXParser is intended to be used alongside [TSXParser](https://github.com/solar-storm-studios/TSXParser) due to internal map tilesets not being supported. Grab the external tileset filepath and give it to TSXParser to get the full tileset info.

#TMXParser

TMXParser is a Tiled Map Editor *.tmx map xml file parser. It is currently a completely functionless library designed to provide instant access to the *.tmx file data. TMXParser can be included (along with rapidxml) into your project or statically linked.

##Features

* Completely functionless giving access to the raw map data.
* Support for multiple:
  * Tilesets
  * Tile Layers
  * Object Groups
  * Objects ( Currently only rectangular and tile based objects )
  * Image Layers
* Complete properies support.

##TODO

* Ellipse
* Polygon
* Polyline
* Tile Flip
* Tile Rotation
* Implement full API
