#ifndef PARSE_H
# define PARSE_H

# ifdef __cplusplus
extern "C" {
# endif

#define MONSTER_DESCRIPTION_VERSIONING "RLG229 MONSTER DESCRIPTION 1"

void parse_monster_descriptions();

# ifdef __cplusplus

class monster_description {
  private:
    std::string m_name, m_description, m_color, m_speed, m_hp, m_dmg;
    char m_symbol;
  public:
    void set_description(std::string name, std::string description, std::string color, std::string speed, std::string hp, std::string dmg, char symbol) {
      m_name = name;
      m_description = description;
      m_color = color;
      m_speed = speed;
      m_hp = hp;
      m_dmg = dmg;
      m_symbol = symbol;
    }
};

}
# endif

#endif
