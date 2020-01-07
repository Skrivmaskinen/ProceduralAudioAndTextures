// ----------
// A simple fireplace. The crackling is somewhat lacking, the impulses needs to be bonks and poofs rather than just impulses.
// ----------
s.boot;
// White noise that is LP-filtered around 100 Hz.
// The cutoff-freq is noised => cutoff-freq = [50, 150] Hz.
{ LPF.ar(WhiteNoise.ar, 100*(1 + (LFDNoise1.kr*0.5)))*(1 + LFDNoise1.kr(10) /4 + 0.5)}.play;
// Impuls noise (Dust). Lowpassfilter with cutoff-freq modulated by exponential noise. Reverbed; big room gives nice effect, not too much wettness.
{ 	FreeVerb.ar(LPF.ar(Dust.ar(10) *((LFDNoise1.kr)), LFDNoise0.kr(20).exprange(3200, 10000)), 0.1, 1.0) }.play
// A few non LP-filtered impulses. Reverbed with same as above.
{ FreeVerb.ar(Dust.ar(2) *((LFDNoise1.kr)), 0.1, 1.0) }.play

