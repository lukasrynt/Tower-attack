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
- Parametry útočníků: znak, barva, rychlost, životy, ...
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
> V ukázkové mapě lze vidět útočníky (__@__), věže (__\*__, __%__) a bránu (__O__)

### Definice souboru
Soubory ve kterých jsou uložené rozehrané hry nebo nové mapy mají podobnou strukturu. Je pevně daná - každý řádek začíná určitým znakem ve formátu `(C):`, kde C je znak který určuje co načítáme, a končí středníkem. Každý znak se může v souboru vyskytovat jen jednou, to zamezuje redefinici dat. Následuje ukázka souboru ve kterém je uložená rozehraná hra.

Na konci souboru je obsažena samotná mapa, za ní už nesmí následovat nic dalšího. Mapa má přípustné rozdílné hodnoty v případě, že se jedná o rozehranou hru (jsou povoleny rozmístěné věže a útočníci).
```
--Zásobník vzorů--
(U)
T @ 20 60 50 30             -- pěšák: znak, cena, život, rychlost, útok
A $ 20 80 40 40 90          -- obrněná jednotka: znak, cena, život, rychlost, útok, brnění
M % 20 20 3 30              -- mágova věž: znak, rychlost, útok, dostřel, rychlost šíření vln
R * 20 20 3                 -- lukostřelecká věž: znak, rychlost, útok, dostřel


--Vlny--
(W)                         -- počet a velikost se odvodí z formátu
0 20 300                    -- značí zda je zrovna spuštěná vlna nebo ne, rychlost spawneru, zdroje dostupné pro hru
[@    ]                     -- včetně uložených jednotek
[     ]

--Mapa--
(M)
 6 5 (20 100)               -- dimenze mapy - řádek, sloupec, brána (aktuální, max život)
#####                       -- mapa
# # O
1   #
#   #
#   #
#####                           --Vojáci a věže na mapě--
@ (10 10) 10                -- znak, pozice, aktuální frame
$ (1 1) 2, 20               -- znak, pozice, aktuální frame, stav brnění
% (2 2) 10                  -- znak, pozice, aktuální frame, aktuální vlna
* (2 2) 10                  -- znak, pozice, aktuální frame
```
> Ukázka souboru
>
### Pravidla hry
Cílem hráče je dostat co největší množství svých jednotek přes věže, které budou na vojáky přirozeně útočit, aby jim zabránily v postupu. V průběhu hry hráč spotřebovává prostředky, díky kterým může vysílat lepší jednotky do boje. V případě, že útočník dorazí až do cíle, poškodí bránu. V případě zabití nepřítele přirozené vyhrává hráč. Prohra nastává v případě, kdy hráči došly všechny prostředky a brána stále stojí.

### Nástin implementace
Hlavní třídá celé hry je `CApplication`, která spouští celý program a main loop. Je v ní třídá `CInterface`, která se stará o komunikaci s uživatelem. Dále obsahuje hru `CGame`, která se stará o chod aktuální hry.
Hra se skládá z více entit, jednou z nich je mapa `CMap`, poté úložiště všech jednotek `CUnitStack` a nakonec vlny s útočníky `CWaves`. K samotnému vykreslování slouží třída `CBuffer`, která do sebe ukládá jednotlivé řádky výpisu a je schopná je centrovat a připojovat k sobě jiné buffery.

### Kde mám polymorfismus
Polymorfismus využívám ve třídě `CTower`, která má virtuální metodu `Attack`. Metoda `Attack` simuluje útok věže na vojáka s tím, že `CTower` je pouze jednoduchá věž s jedním lukostřelcem a vysílá tedy pouze jeden šíp s menším poškozením. Tuto metodu přetěžuje její potomek `CMageTower`, která už implementuje sofistikovanější útok s pomocí temných mágů, kteří všude kolem své věže vysílají elektrické paprsky (`Attack` tedy poškozuje jednotky v celém svém okolí).

Další příklad polymorfismu je ve třídě `CTrooper`, která má virtuální metodu a `ReceiveDamage`. Jejím potomkem je `CArmoredTrooper`. Metoda `ReceiveDamage` je u základního vojáka triviální, ale `CArmoredTrooper` má navíc schopnost se na určitý kroků obrnit a ignorovat velkou část poškození. Další metodou kterou tento potomek implementuje je `Move`, který zajišťuje pohyb jednotky na mapě a případnou aktualizaci jejích stavů. Obrněný voják se pohybuje co nejpřímější cestou k cíli, ale slabší jednotky využívají strategii vyhýbaní se věžím.
