#include "TMXParser.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include "base64.h"

namespace TMX {

  Parser::Parser( const char* filename )
  {
    load( filename );
  }

  Parser::Parser()
  {
  }

  Parser::~Parser()
  {
  }

  bool Parser::load( const char* filename )
  {
    std::string version = VERSION;
    rapidxml::xml_node<>* root_node;
    rapidxml::xml_document<> doc;
    rapidxml::file<> file( filename );
    doc.parse<0>( file.data() );
    //get root nodes
    root_node = doc.first_node( "map" );

    //load map element
    if( root_node->first_attribute( "version" )->value() != version ) {
      std::cout << "ERROR: Map version mismatch. Required version: " << VERSION << "." << std::endl;
      return false;
    }

    mapInfo.version = root_node->first_attribute( "version" )->value();
    std::cout << "Version: " << mapInfo.version << std::endl;
    mapInfo.orientation = root_node->first_attribute( "orientation" )->value();
    std::cout << "Orientation: " << mapInfo.orientation << std::endl;
    mapInfo.width = std::atoi( root_node->first_attribute( "width" )->value() );
    std::cout << "Width: " << mapInfo.width << std::endl;
    mapInfo.height = std::atoi( root_node->first_attribute( "height" )->value() );
    std::cout << "Height: " << mapInfo.height << std::endl;
    mapInfo.tileWidth = std::atoi( root_node->first_attribute( "tilewidth" )->value() );
    std::cout << "Tile Width: " << mapInfo.tileWidth << std::endl;
    mapInfo.tileHeight = std::atoi( root_node->first_attribute( "tileheight" )->value() );
    std::cout << "Tile Height: " << mapInfo.tileHeight << std::endl;

    if( root_node->first_attribute( "backgroundcolor" )->value() != 0 ) {
      mapInfo.backgroundColor = root_node->first_attribute( "backgroundcolor" )->value();
      std::cout << "Background Color: " << mapInfo.backgroundColor << std::endl;
    }

    if( root_node->first_node( "properties" ) != 0 ) {
      for( rapidxml::xml_node<>* properties_node = root_node->first_node( "properties" )->first_node( "property" ); properties_node; properties_node = properties_node->next_sibling() ) {
        mapInfo.property[properties_node->first_attribute( "name" )->value()] = properties_node->first_attribute( "value" )->value();
      }

      std::cout << "Properties: " << std::endl;

      for( std::map<std::string, std::string>::iterator it = mapInfo.property.begin(); it != mapInfo.property.end(); ++it ) {
        std::cout << "-> " << it->first << " : " << it->second << std::endl;
      }
    }

    std::cout << std::endl;

    for( rapidxml::xml_node<>* tileset_node = root_node->first_node( "tileset" ); tileset_node; tileset_node = tileset_node->next_sibling( "tileset" ) ) {
      Tileset tmpTileset;
      tmpTileset.firstGID = std::atoi( tileset_node->first_attribute( "firstgid" )->value() );
      tmpTileset.source = tileset_node->first_attribute( "source" )->value();
      std::cout << "Tileset[ First GID: " << tmpTileset.firstGID << " Source: " << tmpTileset.source << std::endl;
      tilesetList.push_back( tmpTileset );
    }

    for( rapidxml::xml_node<>* layer_node = root_node->first_node( "layer" ); layer_node; layer_node = layer_node->next_sibling( "layer" ) ) {
      TileLayer layer;
      layer.name = layer_node->first_attribute( "name" )->value();
      std::cout << std::endl << "Layer Name: " << layer.name << std::endl;

      if( layer_node->first_node( "properties" ) != 0 ) {
        for( rapidxml::xml_node<>* properties_node = layer_node->first_node( "properties" )->first_node( "property" ); properties_node; properties_node = properties_node->next_sibling() ) {
          layer.property[properties_node->first_attribute( "name" )->value()] = properties_node->first_attribute( "value" )->value();
        }

        std::cout << "Properties: " << std::endl;

        for( std::map<std::string, std::string>::iterator it = mapInfo.property.begin(); it != mapInfo.property.end(); ++it ) {
          std::cout << "-> " << it->first << " : " << it->second << std::endl;
        }
      }

      rapidxml::xml_node<>* data_node = layer_node->first_node( "data" );
      layer.data.encoding = data_node->first_attribute( "encoding" )->value();
      std::cout << "Layer Encoding: " << layer.data.encoding << std::endl;

      if( data_node->first_attribute( "compression" ) > 0 ) {
        layer.data.compression = data_node->first_attribute( "compression" )->value();
        std::cout << "Layer Compression: " << layer.data.compression << std::endl;
      }

      layer.data.contents = data_node->value();
      std::cout << "Layer Data: " << layer.data.contents << std::endl;
      tileLayer[layer.name] = layer;
    }

    for( rapidxml::xml_node<>* oGroup_node = root_node->first_node( "objectgroup" ); oGroup_node; oGroup_node = oGroup_node->next_sibling( "objectgroup" ) ) {
      ObjectGroup oGroup;
      std::cout << std::endl;
      oGroup.color = oGroup_node->first_attribute( "color" )->value();
      std::cout << "Object Group Color: " << oGroup.color << std::endl;
      oGroup.name = oGroup_node->first_attribute( "name" )->value();
      std::cout << "Object Group Name: " << oGroup.name << std::endl;
      oGroup.opacity = std::atof( oGroup_node->first_attribute( "opacity" )->value() );
      std::cout << "Object Group Opacity: " << oGroup.opacity << std::endl;
      oGroup.visible = std::atoi( oGroup_node->first_attribute( "visible" )->value() );
      std::cout << "Object Group Visible: " << oGroup.visible << std::endl;

      if( oGroup_node->first_node( "properties" ) != 0 ) {
        for( rapidxml::xml_node<>* properties_node = oGroup_node->first_node( "properties" )->first_node( "property" ); properties_node; properties_node = properties_node->next_sibling() ) {
          oGroup.property[properties_node->first_attribute( "name" )->value()] = properties_node->first_attribute( "value" )->value();
        }

        std::cout << "Properties: " << std::endl;

        for( std::map<std::string, std::string>::iterator it = mapInfo.property.begin(); it != mapInfo.property.end(); ++it ) {
          std::cout << "-> " << it->first << " : " << it->second << std::endl;
        }
      }

      objectGroup[oGroup.name] = oGroup;
    }

    for( rapidxml::xml_node<>* image_node = root_node->first_node( "imagelayer" ); image_node; image_node = image_node->next_sibling( "imagelayer" ) ) {
      ImageLayer imgLayer;
      std::cout << std::endl;
      imgLayer.name = image_node->first_attribute( "name" )->value();
      std::cout << "Image Layer Name: " << imgLayer.name << std::endl;

      if( image_node->first_attribute( "opacity" ) != 0 ) {
        imgLayer.opacity = std::atof( image_node->first_attribute( "opacity" )->value() );
        std::cout << "Image Layer Opacity: " << imgLayer.opacity << std::endl;
      }

      imgLayer.visible = std::atoi( image_node->first_attribute( "visible" )->value() );
      std::cout << "Image Layer Visibility: " << imgLayer.visible << std::endl;

      imgLayer.image.source = image_node->first_node( "image" )->first_attribute( "source" )->value();
      std::cout << "Image Layer Source: " << imgLayer.visible << std::endl;

      if( image_node->first_node( "image" )->first_attribute( "trans" ) != 0 ) {
        imgLayer.image.transparencyColor = image_node->first_node( "image" )->first_attribute( "trans" )->value();
        std::cout << "Image Layer Transparent Color: " << imgLayer.image.transparencyColor << std::endl;
      }

      if( image_node->first_node( "properties" ) != 0 ) {
        for( rapidxml::xml_node<>* properties_node = image_node->first_node( "properties" )->first_node( "property" ); properties_node; properties_node = properties_node->next_sibling( "property" ) ) {
          imgLayer.property[properties_node->first_attribute( "name" )->value()] = properties_node->first_attribute( "value" )->value();
        }

        std::cout << "Properties: " << std::endl;

        for( std::map<std::string, std::string>::iterator it = imgLayer.property.begin(); it != imgLayer.property.end(); ++it ) {
          std::cout << "-> " << it->first << " : " << it->second << std::endl;
        }
      }

      imageLayer[imgLayer.name] = imgLayer;
    }

    return true;
  }
}
