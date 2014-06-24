# ELD Designer

ELD Designer is a 2D, non-tiled based, level editor designed to be as universal and simple as possible. It contains (and will contain) only things that are general purpose and universal. It will try to stay simple and quick tool for prototyping levels/states, not designing complex gaming mechanics, effects or animations.

There is a screenshot [here](http://solarcoherence.com/?p=92)

## Features

* Hierarchy of objects defining the level structure (Tree of objects);
* All objects and types are custom specified with their properties based on designers needs;
* Edit multiple levels in one application instance;
* Arbitrary placement of graphical assets/objects on a canvas;
* Free roation of objects and assets;
* Autosized canvas;

Definetly, many heavily used features are still missing, but i guess they will find their way :)

## Disclaimer

Please, be aware that this application is in an early stage of development. It is developed as fast as possible to serve its purpose in the real world, not as a hobby project. Do not expected it to be a fancy polished software.

Based on my research for such tools it appears to contain most of the basic (and universal) stuff for the most time consuming tasks when designing levels for aracade, jump&rund and action games - placements of the assets and objects, definig zones and triggers.

## Building an executable

The application is developed in QT 5.3, visit [QT Project](http://qt-project.org/) for more info. I am developing and testing it on Windows, but it should be easy to build it for any other platform like Mac or Linux.

I am not releasing an executable yet, but if you don't like to mess with QT i can always send you one, just leave me a note!

## Bindings with engines and frameworks

In short - currently none :(

The application saves its levels in a JSON format. The output files should be parsed and processed by the engine/framework/language you are using. The format is very simple! It reflects the logic which you put into the configuration of the designer itself.

Here is an example of a such output file:
```
{
    "children": [
        {
            "children": [
                {
                    "children": [
                        {
                            "id": "Ship-3",
                            "properties": {
                                "rotation": "37.213",
                                "texture": "D:/projects/my_game/code/asset/image/ship/ship1.png",
                                "unit": "ship1",
                                "x": "271",
                                "y": "286"
                            },
                            "type": "Ship"
                        }
                    ],
                    "id": "Fleet-2",
                    "type": "Fleet"
                }
            ],
            "id": "Layer-1",
            "properties": {
                "depth": "1"
            },
            "type": "Layer"
        }
    ],
    "type": "Level"
}
```

## Configuration

The cool thing about this level editor is that it is very flexible because there are no standart types and properties. Everything is described in a JSON configuration file and then used to design the levels.

So you cerate a json file and place it anywhere you like. Then be sure to point the location of the file after you launch the application (From the Tools menu)!

Please, note that the application will not report parse errors! So it is best to run your json file through validator like (JSONLint)(http://jsonlint.com/) in case you are confused why your file is not loaded!

Here is an exmaple of a typical configuration.json
```
{
	"objects": [
		{
			"type": "Level",
			"children": ["LayerType1"]
		},
		{ 
			"type": "LayerType1",
			"children": ["ObjectType1", "ZoneType1"]
		},
		{
			"type": "ObjectType1",
			"properties": ["x", "y", "width", "texture"]
		},
		{
			"type": "ZoneType1",
			"width": "100",
			"height": "100",
			"properties": ["height", "rotation"]
		}
	],
	"properties": [
		{ 
			"name": "x",
			"type": "int" 
		},
		{ 
			"name": "y",
			"type": "int" 
		},
		{ 
			"name": "width",
			"type": "int" 
		},
		{ 
			"name": "height",
			"type": "int" 
		},
		{ 
			"name": "rotation",
			"type": "number" 
		},
		{
			"name": "unit",
			"type": "combo",
			"options": [
				{
					"type": "UnitType1", 
					"$texture": "path to image here"
				},
				{
					"type": "UnitType2",
					"$texture": "path to image here"
				}
			]
		},
		{
			"name": "texture",
			"type": "file",
			"meta": "*.png,*.jpg,*.jpeg,*.bmp"
		}
	]
}
```

## Other things to be noted

UI icons by [FAM FAM FAM](http://www.famfamfam.com/lab/icons/silk/). Awesome icon pack!

The aplication is developed to be used as the level editor for [Solar Coherence](http://solarcoherence.com), a space strategy/arcade game.

The word "ELD" means Eldhelm-Level-Designer or just Eldhelm. The name comes from the pilot project [Battlegrounds of Eldhelm](http://eldhelm.essenceworks.com) of my start-up company [Essence](http://essenceworks.com). I am starting to brand with the word Eldhelm all the tools and frameworks i design and build to aid me in my quest for creating outstanding indie games!

Cheers!