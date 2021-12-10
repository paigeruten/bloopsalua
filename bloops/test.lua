require("bloops")

b = Bloops:new()
b.tempo = 320

saw = b:sound(Bloops.SAWTOOTH)
saw:test()

b:sleep(1)

b:play("c5 c6 b4 b5 d5 d6 e5 e6")
