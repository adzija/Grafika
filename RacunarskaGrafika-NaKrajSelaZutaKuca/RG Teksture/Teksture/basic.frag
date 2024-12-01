#version 330 core //Koju verziju GLSL jezika da koristim (za programabilni pipeline: GL verzija * 100) i koji profil (core - samo programabilni pipeline, compatibility - core + zastarjele stvari)

//Kanali (in, out, uniform)
in vec4 channelCol; //Kanal iz Verteks sejdera - mora biti ISTOG IMENA I TIPA kao u vertex sejderu
in vec2 texCoord; //koordinate teksture

out vec4 outCol; //Izlazni kanal koji ce biti zavrsna boja tjemena (ukoliko se ispisuju podaci u memoriju, koristiti layout kao za ulaze verteks sejdera)

uniform sampler2D uTex; //teksturna jedinica
uniform bool useTexture;

void main() //Glavna funkcija sejdera
{
    outCol = channelCol;

    if (useTexture) {
        outCol = texture(uTex, texCoord); //boja na koordinatama chTex teksture vezane na teksturnoj jedinici uTex
    } 
    
}