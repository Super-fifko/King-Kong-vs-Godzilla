#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "rng.h"
#include "data.h"

long int string_na_long_int(char*string);

void vypis_stavu_jednotiek(Unit monster, Unit nepriatelia[], int pocet_jednotiek);

int vypocet_dmg(Unit attacker,Unit deffender);

int monster_target(Unit nepriatelia[], int pocet_jednotiek);

int main(int argc,char *argv[]) {
    int count = 0;
    long int pocet_jednotiek = string_na_long_int(argv[2]);
    long int seed = string_na_long_int(argv[3]);
    srnd(seed);

    UnitType mon;
    Unit monstrum;
    for (int i = 0; i < MONSTER_TYPE_COUNT; i++) {                          //Inicializacia monstra
        mon = monster_types[i];
        if (strcmp(mon.name, argv[1]) == 0) {
            monstrum.type = &mon;
            monstrum.hp = MONSTER_INITIAL_HP;
            monstrum.level = 0;
            break;
        }
        count++;
        if (count == 3) {                                               //error num.1
            return 1;
        }
    }


    Unit nepriatelia[pocet_jednotiek];
    Unit nepriatel;
    UnitType inic_nepriatel;
    if (argc == 6) {
        FILE *doc;
        doc = fopen(argv[5], "r");
        if (doc == NULL)           //error num.2
        {
            return 2;
        } else {
            for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {

                if (fscanf(doc, "%s %d %d", inic_nepriatel.name, &inic_nepriatel.att, &inic_nepriatel.def) == 3) {
                    enemy_types[i] = inic_nepriatel;

                } else {
                    return 3;                       //error num.3
                }
            }
        }
        fclose((FILE *) argv[5]);
    }


    for (int i = 0; i < pocet_jednotiek; i++) {                             //inicializacia nepriatelskych jednotiek
        nepriatel.type = &enemy_types[rnd(0, ENEMY_TYPE_COUNT - 1)];
        nepriatel.hp = rnd(ENEMY_MIN_INIT_HP, ENEMY_MAX_INIT_HP);
        nepriatel.level = rnd(0, UNIT_MAX_LEVEL);
        nepriatelia[i] = nepriatel;
    }


    vypis_stavu_jednotiek(monstrum, nepriatelia, pocet_jednotiek);    //Pociatocny vypis pred bojom
    printf("\n");
    int index_monster_target, dmg,alive_unit_index,total_monster_dmg=0,total_enemies_dmg=0,winner_ID;

    while (1) {                                 //zaciatok boja
        alive_unit_index=0;                 //pocet zivych jednotiek pocas kola
        index_monster_target = monster_target(nepriatelia, pocet_jednotiek);         //pole nepriatelia, pocet_jednotiek
        dmg = vypocet_dmg(monstrum, nepriatelia[index_monster_target]);
        total_monster_dmg+=dmg;                                                      //pripocet dmg monstra
        nepriatelia[index_monster_target].hp = nepriatelia[index_monster_target].hp-dmg;        //odpocet zivota targetnutej jednotky
        printf("%s => %d => [%d] %s\n",monstrum.type->name,dmg,index_monster_target,nepriatelia[index_monster_target].type->name);
        for(int i=0; i<pocet_jednotiek;i++){
            if(nepriatelia[i].hp>0)
            {
                dmg = vypocet_dmg(nepriatelia[i],monstrum);
                total_enemies_dmg+=dmg;
                monstrum.hp=monstrum.hp-dmg;
                printf("[%d] %s => %d => %s\n",i,nepriatelia[i].type->name,dmg,monstrum.type->name);
                alive_unit_index++;
            }
            if (monstrum.hp<=0){
                break;
            }
        }
        printf("\n");

        if (monstrum.hp<=0){                        //pripad vyhry nepriatelov
            winner_ID=0;
            break;
        }
        if(monstrum.level<UNIT_MAX_LEVEL){
            monstrum.level++;
        }
        if(alive_unit_index==0){                    //pripad vyhry monstra
            winner_ID=1;
            break;
        }
        vypis_stavu_jednotiek(monstrum, nepriatelia, pocet_jednotiek);
        printf("\n");
    }
    vypis_stavu_jednotiek(monstrum, nepriatelia, pocet_jednotiek);
    printf("\n");
    if(winner_ID==1){
        printf("Winner: %s\n",monstrum.type->name);
    }
    else{
        printf("Winner: Enemy\n");
    }
    printf("Total monster DMG: %d\n",total_monster_dmg);
    printf("Total enemies DMG: %d",total_enemies_dmg);

    return 0;
}






long int string_na_long_int(char*string){
    long int cislo;
    char * endPtr;
    cislo=strtol(string,&endPtr,10);
    return cislo;
}


void vypis_stavu_jednotiek(Unit monster, Unit nepriatelia[], int pocet_jednotiek){
    int i;
    printf("%s, ATT:%d, DEF:%d, HP:%d, LVL:%d\n",monster.type->name,monster.type->att,monster.type->def,monster.hp,monster.level);
    for(i=0;i<pocet_jednotiek;i++){
        printf("[%d] %s, ATT:%d, DEF:%d, HP:%d, LVL:%d\n",i,nepriatelia[i].type->name,nepriatelia[i].type->att,nepriatelia[i].type->def,nepriatelia[i].hp,nepriatelia[i].level);
    }
}
int vypocet_dmg(Unit attacker,Unit deffender){
    int base_dmg = 30 + attacker.level - deffender.level;
    int c1,c2,d1,d2;
    c1 = rnd(1,attacker.hp);
    c2 = attacker.type->att;
    int strength = 100 + c1 + c2;
    d1 = rnd(1,deffender.hp);
    d2 = deffender.type->def;
    int deffence = 100 + d1 + d2;
    int dmg = (base_dmg * strength)/deffence;
    return dmg;
}

int monster_target(Unit nepriatelia[], int pocet_jednotiek){
    int i,target=10000,target_index;
    for(i=0;i<pocet_jednotiek;i++){
        if(nepriatelia[i].hp<target&&nepriatelia[i].hp>0){
            target=nepriatelia[i].hp;
            target_index=i;
        }
    }
    return target_index;
}
