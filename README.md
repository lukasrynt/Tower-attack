# Incursion (téma Tower attack)

## Téma z progtestu
Naprogramujte jendoduchou grafickou tower ATTACK hru (nejedná se o tower defense)

Váš engine:

1. ze souboru nahraje definici ÚTOČNÍKŮ (jméno, životy, rychlost, útok, odolnost na určitý typ věže ...)
2. ze souboru nahraje možné mapy a typy obranných věží (životy, rychlost, dosah, útok, odolnost na určitý typ věže ,...)
3. implementuje jednoduchou interakci věž vs. útočníci (útok, proběhnutí, ...), počitadlo skóre, detekci vítězství (po nezabití x útočníků)
4. implementuje jednoduchou AI řídící rozmisťování věží
5. umožňuje ukládat a načítat rozehrané hry
Engine může fungovat jako real-time hra, či tahová hra.

Jak může vypadat mapa?

" "prázdné místa pro pohyb útočníku a stavbu věží, I označuje věže, # označuje zeď, @ a % jsou různé druhy útočníků, <= jsou vstupy/výstupy z mapy.

 ```
#################################################
#                        #       @@  #   @@  @@<=3
#         #              #    %I  @  ###        #
#         #              #    %#  @    #    I   #
<=%%%     #              I    %#       #@  @@  <=1
#         #              I    %#       @@   I   #
#                        I    %#                #
#                 %%%       %%%#    @          <=2
#################################################    
```
Cesta kudy se budou útočníci ubírat bude vždy nejkratší možná vzhledem ke zdím a věžím. Můžete navíc implementovat více strategií procházení, kde každý útočník bude mít svoji strategii. Například slabší marvenci se mohou snažit vyhýbat věžím, naopak odolnější zase mohou být pomalejší a vždy musí udělat alespoň X kroků rovně.

Hráč volí vstup a typy útočníků

Kde lze využít polymorfismus? (doporučené)
- Parametry útočníků: znak, barva, rychlst, životy, ...
- Efekty útoku věží: zranění, zpomalení, ...
- Políčka mapy: volno, věž, útočník ...
- Uživatelské rozhraní: konzole, ncurses, SDL, OpenGL (různé varianty), ...

Ukázky:
- http://en.wikipedia.org/wiki/Tower_defense (tower-attack opačná varianta od tower-defense)
- http://store.steampowered.com/app/251530/
- http://www.youtube.com/watch?v=4eV4918kReE
- http://www.newgrounds.com/portal/view/576124

## Zadání hry Incursion
Vytvoříme hru, která se spustí v konzoli a reaguje na stisk jednotlivých kláves, ne celých příkazů. Po startu se spustí úvodní menu ve kterém má hráč několik možností:
1. *Nová hra* - načte novou hru ze souboru s příponou _map_, který má specifický formát (viz dále)
2. *Načtení uložené hry* - načte hru, která byla v některém předchozím běhu uložena do souboru. Soubor je zakončen příponou _sav_
3. *Ukončení* - tato možnost je k dispozici v průběhu celého běhu programu krom zadávání jména mapy pomocí klávesy _q_

Po načtení mapy a definice útočníku ze souboru se spustí samotná hra. Ve vrchní části obrazovky jsou vidět vlny do kterých může uživatel přidávat útočníky, zároveň je zde výběr útočníků a nakonec současný stav prostředků hráče a životy nepřítele. V dolní části obrazovky je samotná mapa, například v následujícím formátu:
```
#############################
1    #         #            #
# ## #  #%###  #  #######   #
# #  #      #     # #       #
# ######### ##*#  # #    ####
#  #      #    #    #    #  #
##     ####    ######    #  #
#    ###@@@  #   #          #
#        #   #####    #######
#        #     #            #
#####################O#######
```
> V ukázkové mapě lze vidět útočníky (__@__), věže (__\*__, __%__) a nepřítele (__O__)

### Definice souboru
Soubory ve kterých jsou uložené rozehrané hry nebo nové mapy mají podobnou strukturu. Je pevně daná - každý řádek začíná určitým znakem ve formátu `(C):`, kde C je znak který určuje co načítáme, a končí středníkem. Každý znak se může v souboru vyskytovat jen jednou, to zamezuje redefinici dat. Přípustné jsou následující znaky:
-_M_: __Mapa__, následují dimenze mapy
-_G_: __Gate__, následují životy brány
-_@_, _$_: __Trooper__, následují parametry vojáka, na základě jeho typu
-_%_, _*_: __Tower__, následují parametry věže, na základě jejího typu

Na konci souboru je obsažena samotná mapa, za ní už nesmí následovat nic dalšího. Mapa má přípustné rozdílné hodnoty v případě, že se jedná o rozehranou hru (jsou povoleny rozmístěné věže a útočníci).
```
(@): 60, 50, 30;
($): 80, 40, 40, 90;
(%): 20, 20, 30, 60;
(*): 20, 20, 30;
(G): 2;
(M): 5,5;
#####
1 # O
# # #
2   #
#####
```
> Ukázka souboru
### Pravidla hry
Cílem hráče je dostat co největší množství svých jednotek přes věže, které budou na vojáky přirozeně útočit, aby jim zabránily v postupu. V průběhu hry hráč střádá prostředky, díky kterým může vysílat lepší jednotky do boje. V případě, že útočník dorazí až do cíle, poškodí nepřítele. V případě zabití nepřítele přirozené vyhrává hráč. Prohra nastává v případě, kdy hráči došly všechny prostředky.

### Nástin implementace
Hlavní třídá celé hry je `CApplication`, ve které se odehrává komunikace s uživatelem, obsahuje mapu `CMap`, ukazatel na úložiště všech jednotek `CUnitStack` a nakonec vlny s útočníky `CWaves`. Dále se stará o aktualizaci mapy, renderování jednotlivých objektu přes jejich metodu `Render`, ukládání a načítání ze souborů.

### Kde mám polymorfismus
Polymorfismus využívám ve třídě `CTower`, která má virtuální metodu `Attack`. Metoda `Attack` simuluje útok věže na vojáka s tím, že `CTower` je pouze jednoduchá věž s jedním lukostřelcem a vysílá tedy pouze jeden šíp s menším poškozením. Tuto metodu přetěžuje její potomek `CMageTower`, která už implementuje sofistikovanější útok s pomocí temných mágů, kteří všude kolem své věže vysílají elektrické paprsky (`Attack` tedy poškozuje jednotky v celém svém okolí).

Další příklad polymorfismu je ve třídě `CTrooper`, která má virtuální metodu a `ReceiveDamage`. Jejím předkem je `CArmoredTrooper`. Metoda `ReceiveDamage` je u základního vojáka triviální, ale `CArmoredTrooper` má navíc schopnost se na určitý kroků obrnit a ignorovat velkou část poškození.
