44100 cycles / s sample rate
lägsta ljudfrekvensen är 20hz, för att känna igen detta,

20 cycles/s -> 1/20 = 0.05s / cycle -> sampla i 0.1s. Detta innebär 0.1*44100 samples = 4410 samples

Om vi istället samplar i 0.25s -> 0.25*44100 = 11025 samples

N = 11025, Fs = 44100 -> Fs/N = 4.


Sample rate: 10000Hz
För att känna igen lägsta -> sampla i 0.1s, vilket innebär 0.1*10000 = 1000 samples


294117 Hz sample rate -> 29412 samples in 0.1 s.
SKIPPA VAR X:te för att få 9000Hz!
---

Bara en cycle -> 9000Hz sample rate för max 4186Hz ljud. Vi vill känna igen 60Hz - 4186Hz.
1/60 = 0.01666666666s per cycle. 0.02 blir bra.

0.02*9000 = 180 samples.

---

9000/N = 8 -> N = 1125.
-> Vi samplar i 1125*1/9000 = 0.125s

---

Om vi vill ha 2^x = N som är högre än 1125, så får vi ta N = 2048.
Om vi har sample rate = 8192 Hz då är varje intervall 4Hz.

Om vi tar SPI1BRG = 8, då är baud rate ~294117Hz. Om vi skippar och tar var 36:e sample så blir sample rate ~8170Hz.
