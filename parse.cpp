#include <iostream>

#include "parse.h"
void parse_monster_descriptions()
{
  std::string filename, line;
  std::ifstream fin;

  filename = getenv("HOME");
  filename += "/.rlg229/monster_desc.txt";
  fin.open(filename); //open file
  if (!fin.good()) {
    std::cout << "Monster description text file expected, but not found. Check for file and path mismatch in parse.cpp" << std::endl;
    return; //exit if file not found
  }

  // checking monster description versioning
  getline(fin, line);
  if (line != MONSTER_DESCRIPTION_VERSIONING)
  {
    std::cout << "Monster description versioning incorrect." << std::endl;
    return;
  }
  getline(fin, line);

  // should be start of a monster
  while(!fin.eof())
  {
    getline(fin, line);
    if (line == "BEGIN MONSTER")
    {
      parse_monster_description(fin, line);
    }
  }
}
