==============
||  README  ||
==============

This program follows all rules set forth by the assignment PDF. It creates a
randomly generated dungeon with 12 room of varying sizes. All rooms are
connected by cooridors.

/*
 * Structure:   cell
 * ---------------------
 * the cell contains a symbol that can be printed to the console,
 * a hardness rating, and a mutable flag
 */

/*
 * Structure:   room
 * ---------------------
 * the room contains x and y values (coordinates) for both the start
 * of the room and the end of the room
 */

/*
 * Structure:   dungeon
 * ---------------------
 * the dungeon structure contains a 160 x 96 array of cells and 
 * an array of 12 rooms
 */

/*
 * Function:    print_dungeon
 * --------------------------
 * prints the dungeon map to the console
 */

/*
 * Function:    is_outermost_wall
 * ------------------------------
 * checks whether the given x and y values are part of the dungeon border
 */

/*
 * Function:    passes_hausdorff
 * -----------------------------
 * checks whether the given room is further than hausdorff's distance from all
 * other rooms in the given dungeon
 */

/*
 * Function:    create_room
 * ------------------------
 * creates a room and adds it to the given dungeon if it has all the valid
 */

/*
 * Function:    connect_points
 * ----------------------------
 * given a dungeon and two points, connects the two points in the dungeon
 */

/*
 * Function:    create_corridors
 * ------------------------------
 * loops through the rooms stored in a given dungeon and connects
 * the rooms one by one
 */

/*
 * Function:    generate_dungeon
 * -----------------------------
 * generates a random dungeon
 */