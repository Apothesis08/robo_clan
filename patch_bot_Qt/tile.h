#ifndef TILE_H
#define TILE_H

#include <unordered_map>

// Class tile stores enums of tiles
// creates unordered_map of characters and tile enums,
// used in map class to create 2D enum tile vector
class tile {

public:
    // enum of tiles
    enum tile_enum {
        boden_Klar = 10, boden_GrunPatch, boden_RotPatch,
        //Gefahren
        gefahr_Abgrunde, gefahr_Wasser,
        // Hauptpunkte
        punkte_Start, punkte_Hauptserver,
        // Hindernisse
        hind_Aliengras, hind_Schrotter, hind_Geheimgange,
        // Taren
        tur_Blau, tur_Rot, tuer_manuell_offen, tuer_automatisch_offen,
        // Wande
        wand_BetonGrau, wand_FelsBraun
    };

    // unordered_map of tile enum
    std::unordered_map<char, tile_enum> tile_table;

    // initialise tile enum unordered_map
    tile()
    {
        // unordered map for background character and respective enums
        tile_table[' '] = tile_enum{ boden_Klar };
        tile_table['p'] = tile_enum{ boden_GrunPatch };
        tile_table['1'] = tile_enum{ boden_RotPatch };
        tile_table['2'] = tile_enum{ boden_RotPatch };
        tile_table['3'] = tile_enum{ boden_RotPatch };
        tile_table['4'] = tile_enum{ boden_RotPatch };
        tile_table['5'] = tile_enum{ boden_RotPatch };
        tile_table['6'] = tile_enum{ boden_RotPatch };
        tile_table['7'] = tile_enum{ boden_RotPatch };
        tile_table['O'] = tile_enum{ gefahr_Abgrunde };
        tile_table['~'] = tile_enum{ gefahr_Wasser };
        tile_table['P'] = tile_enum{ punkte_Hauptserver };
        tile_table['g'] = tile_enum{ hind_Aliengras };
        tile_table['.'] = tile_enum{ hind_Schrotter };
        tile_table['x'] = tile_enum{ hind_Geheimgange };
        tile_table['d'] = tile_enum{ tur_Blau };
        tile_table['D'] = tile_enum{ tur_Rot };
        tile_table['#'] = tile_enum{ wand_BetonGrau };
        tile_table['M'] = tile_enum{ wand_FelsBraun };
    }

    // size of image icon
    int tile_size = 32;

};

#endif // TILE_H
