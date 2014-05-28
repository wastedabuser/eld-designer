# ELD Designer

ELD DEsigner is a 2D non-tiled based level editor designed to be as universal and simple as possible. It contains (and will) contain only things that are general purpose and universal. It will be a simple and quick tool for prototyping level states, not designing complex gaming mechanics, effects or animations.

## Features

* Herirachy of objects defining the level structure
* All objects and types are custom specified with their properties based on designers needs.
* Multiple documents in one application instance
* Arbitrary placing of graphical assets over a canvas
* Free roation of these assets
* Autosized canvas

Definetly, many heavily used stuff are missing, but i guess they will find their way :)

## Disclaimer

Please, be aware that this application is in an early stage of development. It is developed as fast as possible to serve its purpose in the real world! It is not a fancy polished software.

Based on my research for such tools it appears to contain the basic (and universal) stuff for the most time consuming tasks when designing levels for  aracade, jump&rund and action games.

## Building

The application is developed in QT 5.3. You should be able to build it for any platform.

## Bindings

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

Please, be sure to point the configuration file location after you launch the application!

Here is exmaple of a typical configuration.json
```
{
	"rootObject": {
		"type": "Level",
		"children": ["LayerType1"]
	},
	"objects": [
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
	"properties": {
		"x": { 
			"name": "x",
			"type": "int" 
		},
		"y": { 
			"name": "y",
			"type": "int" 
		},
		"width": { 
			"name": "width",
			"type": "int" 
		},
		"height": { 
			"name": "height",
			"type": "int" 
		},
		"rotation": { 
			"name": "rotation",
			"type": "number" 
		},
		"unit": {
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
		"texture": {
			"name": "texture",
			"type": "file",
			"meta": "*.png,*.jpg,*.jpeg,*.bmp"
		}
	}
}
```

## Other things

UI icons by [FAM FAM FAM](http://www.famfamfam.com/lab/icons/silk/)

The aplication is developed to be used as the level editor for [Solar Coherence](http://solarcoherence.com), a strategy/arcade game.

The word "ELD" means Eldhelm-Level-Designer or just Eldhelm. The name comes from the pilot project [Battlegrounds of Eldhelm](http://eldhelm.essenceworks.com) of my start-up gamedev company [Essence](http://essenceworks.com). I am branding with the word Eldhelm all my tools and frameworks designed and built to aid in my quest to create outstanding indie games!

Cheers!