44100 cycles / s sample rate
lägsta ljudfrekvensen är 20hz, för att känna igen detta,

20 cycles/s -> 1/20 = 0.05s / cycle -> sampla i 0.1s. Detta innebär 0.1*44100 samples = 4410 samples

Om vi istället samplar i 0.25s -> 0.25*44100 = 11025 samples

N = 11025, Fs = 44100 -> Fs/N = 4.


Sample rate: 10000Hz
För att känna igen lägsta -> sampla i 0.1s, vilket innebär 0.1*10000 = 1000 samples
