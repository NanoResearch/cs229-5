#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "parse.h"

void parse_monster_description(std::ifstream &fin, std::string &line)
{
  std::string name, desc, symb, color, speed, abil, hp, dam;
  std::string token;

  while (!fin.eof())
  {
    fin >> token;

    // name
    if (token == "NAME")
    {
      if (name != "")
      {
        return;
      }
      fin.get();
      getline(fin, line);
      name = line;
    }

    // symb
    else if (token == "SYMB")
    {
      if (symb != "")
      {
        return;
      }
      fin.get();
      getline(fin, line);
      symb = line;
    }

    // color
    else if (token == "COLOR")
    {
      if (color != "")
      {
        return;
      }
      fin.get();
      getline(fin, line);
      color = line;
    }

    // speed
    else if (token == "SPEED")
    {
      if (speed != "")
      {
        return;
      }
      fin.get();
      getline(fin, line);
      speed = line;
    }

    // dam
    else if (token == "DAM")
    {
      if (dam != "")
      {
        return;
      }
      fin.get();
      getline(fin, line);
      dam = line;
    }
    
    // hp
    else if (token == "HP")
    {
      if (hp != "")
      {
        return;
      }
      fin.get();
      getline(fin, line);
      hp = line;
    }
    
    // abil
    else if (token == "ABIL")
    {
      if (abil != "")
      {
        return;
      }
      fin.get();
      getline(fin, line);
      abil = line;
    }

    // desc
    else if (token == "DESC")
    {
      if (desc != "")
      {
        return;
      }
      while (!fin.eof())
      {
        getline(fin, line);
        if (line == ".")
        {
          break;
        }
        desc += line;
        desc += "\n";
      }
    }

    // finish up and print monster details if valid
    else if (token == "END")
    {
      std::cout << name;
      std::cout << desc;
      std::cout << symb << std::endl;
      std::cout << color << std::endl;
      std::cout << speed << std::endl;
      std::cout << abil << std::endl;
      std::cout << hp << std::endl;
      std::cout << dam << std::endl;
      std::cout << std::endl;
      return;
    }
    else
    {
      return;
    }
  }
}

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
