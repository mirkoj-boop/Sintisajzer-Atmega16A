# 🎹 Razvoj jednostavnog digitalno upravljanog sintisajzera temeljenog na mikroupravljaču ATmega16A

**** jednostavni PCB dizajn:
- **ATMEGA16-A** kao digitalni Oscilator
- an **LM386 preamplifier** pretpojačalo za 8 Ω zvučnik.

Dizajnirano jednostavno pojačalo za sintisajzer

1. Inicijaliziraj I2C komunikaciju
2. Inicijaliziraj tri MCP23008 čipa (postavi sve pinove kao ulaze)
3. Beskonačno ponavljaj:
   a. Pročitaj stanje tipki s prva tri MCP23008 čipa (8 tipki po čipu, ukupno 24)
   b. Inverzija očitanja (jer su tipke aktivne na LOW)
   c. Provjeri pritisnute tipke u sljedećem redoslijedu:
      - Za prvi čip (tipke 0-7):
        Ako je neka tipka pritisnuta, pošalji vrijednost note (notes[0-7]) na DAC i prekini
      - Ako nema pritisnutih tipki na prvom čipu, provjeri drugi čip (tipke 8-15):
        Ako je neka tipka pritisnuta26, pošalji vrijednost note (notes[8-15]) na DAC i prekini
      - Ako nema pritisnutih tipki na drugom čipu, provjeri treći čip (tipke 16-23):
        Ako je neka tipka pritisnuta, pošalji vrijednost note (notes[16-23]) na DAC i prekini
      - Ako nijedna tipka nije pritisnuta, postavi DAC na 0 (tišina)
   d. Pričekaj 10 ms prije sljedećeg očitavanja
