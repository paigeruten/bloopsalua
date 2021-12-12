require("bloops")

b = Bloops:new()
b.tempo = 320

saw = b:sound(Bloops.SAWTOOTH)
saw:test()
b:sleep(1)

b:tune(saw, "c5 c6 b4 b5 d5 d6 e5 e6")

b:play()
while not b:is_stopped() do b:sleep(1) end

beat = b:sound(Bloops.NOISE)
beat["repeat"] = 0.6
beat2 = b:sound(Bloops.NOISE)
beat2["repeat"] = 0.2
beat3 = b:sound(Bloops.SQUARE)
beat3.sustain = 0.25
beat3.decay = 0.2
beat3.slide = 0.2
beat3.square = 0.3
beat3.vibe = 0.25
beat3.vspeed = 0.25

b:tune(beat, "4 4 4 b4 4 d5 4 e5")
b:tune(beat2, "c2 4 c2 4 c2 4 c2 4")
b:tune(beat3, "4 4 4 4 4 c2 c5 4")

while true do
  b:play()
  saw:test()
  while not b:is_stopped() do b:sleep(0.02) end
end
