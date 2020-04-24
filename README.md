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
Vytvoříme hru, která se spustí v konzoli a můžeme jí ovládat pomocí stisku jednotlivých kláves


