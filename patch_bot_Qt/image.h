#ifndef IMAGE_H
#define IMAGE_H

#include <QString>

#include "robot.h"
#include "tile.h"

// Class image to store image of each and every object of the map
// Used to load respective images onto map from unordered_maps
class image {

public:
    // unordered_maps to store image enum and their respective file links
    std::unordered_map<tile::tile_enum, QString> image_table_tile; // image table for background
    std::unordered_map<robot::robot_enum, QString> image_table_robot; // image table for robots

    // initialise images of enum in unordered_map
    image()
    {
        image_table_tile[ tile::tile_enum::boden_Klar             ] = QString{ "umgebungen/boden.png"                         };
        image_table_tile[ tile::tile_enum::boden_GrunPatch        ] = QString{ "umgebungen/boden_start_patchbot.png"          };
        image_table_tile[ tile::tile_enum::boden_RotPatch         ] = QString{ "umgebungen/boden_start_gegner.png"            };
        image_table_tile[ tile::tile_enum::gefahr_Abgrunde        ] = QString{ "umgebungen/gefahr_abgrund.png"                };
        image_table_tile[ tile::tile_enum::gefahr_Wasser          ] = QString{ "umgebungen/gefahr_wasser.png"                 };
        image_table_tile[ tile::tile_enum::punkte_Hauptserver     ] = QString{ "umgebungen/hauptserver.png"                   };
        image_table_tile[ tile::tile_enum::hind_Aliengras         ] = QString{ "umgebungen/hindernis_aliengras.png"           };
        image_table_tile[ tile::tile_enum::hind_Schrotter         ] = QString{ "umgebungen/hindernis_schotter.png"            };
        image_table_tile[ tile::tile_enum::hind_Geheimgange       ] = QString{ "umgebungen/hindernis_geheimgang.png"          };
        image_table_tile[ tile::tile_enum::tur_Blau               ] = QString{ "umgebungen/tuer_manuell_geschlossen.png"      };
        image_table_tile[ tile::tile_enum::tur_Rot                ] = QString{ "umgebungen/tuer_automatisch_geschlossen.png"  };
        image_table_tile[ tile::tile_enum::wand_BetonGrau         ] = QString{ "umgebungen/wand_beton.png"                    };
        image_table_tile[ tile::tile_enum::wand_FelsBraun         ] = QString{ "umgebungen/wand_fels.png"                     };
        image_table_tile[ tile::tile_enum::tuer_automatisch_offen ] = QString{ "umgebungen/tuer_automatisch_offen.png"        };
        image_table_tile[ tile::tile_enum::tuer_manuell_offen     ] = QString{ "umgebungen/tuer_manuell_offen.png"            };

        image_table_robot[ robot::robot_enum::bot_Bugger           ] = QString{ "roboter/typ1_bugger.png"                      };
        image_table_robot[ robot::robot_enum::bot_Pusher           ] = QString{ "roboter/typ2_pusher.png"                      };
        image_table_robot[ robot::robot_enum::bot_Digger           ] = QString{ "roboter/typ3_digger.png"                      };
        image_table_robot[ robot::robot_enum::bot_Swimmer          ] = QString{ "roboter/typ4_swimmer.png"                     };
        image_table_robot[ robot::robot_enum::bot_Follower         ] = QString{ "roboter/typ5_follower.png"                    };
        image_table_robot[ robot::robot_enum::bot_Hunter           ] = QString{ "roboter/typ6_hunter.png"                      };
        image_table_robot[ robot::robot_enum::bot_Sniffer          ] = QString{ "roboter/typ7_sniffer.png"                     };
        image_table_robot[ robot::robot_enum::no_bot               ] = QString{ "umgebungen/boden.png"                         };
        image_table_robot[ robot::robot_enum::patch_bot            ] = QString{ "roboter/patchbot.png"                         };
    }

};

#endif // IMAGE_H
