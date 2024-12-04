#version 330 core //Koju verziju GLSL jezika da koristim (za programabilni pipeline: GL verzija * 100) i koji profil (core - samo programabilni pipeline, compatibility - core + zastarjele stvari)

//Kanali (in, out, uniform)
layout(location = 0) in vec2 inPos; //Pozicija tjemena
layout(location = 2) in vec4 inCol; //Boja tjemena - ovo saljemo u fragment sejder
layout(location = 1) in vec2 inTex;   // Texture coordinates

out vec4 channelCol; //Izlazni kanal kroz koji saljemo boju do fragment sejdera
out vec2 texCoord;


uniform vec2 sunPos;
uniform vec2 smokePos;

uniform vec2 dogPos;
uniform int dogDirection; // Dog's direction (1 or -1)
uniform vec2 zPos; // "Z" position
uniform float zAlpha; // "Z" opacity (alpha value)
uniform float transparency; // Transparency uniform

uniform int mode; //0 sky, 1 sun and moon, 2 grass, 3 texture
uniform bool isSun; // 1 sun, 2 moon
uniform float time;
uniform float windowAnimationProgress;

uniform float paintProgress; // Progress of the painting [0.0, 1.0]

uniform vec2 foodPos;                 // Position of food
uniform float foodScale = 2;       // Size of food (default is 0.05 for square food)
void main() 
{
    switch (mode) {
        case 0:
            vec4 lightBlue = vec4(0.53, 0.81, 0.98, 1.0);
            vec4 darkBlueColor = vec4(0.0, 0.0, 0.2, 1.0);
            float t = (sunPos.y + 1.0) / 2.0;
            vec4 finalBackgroundColor = mix(lightBlue, darkBlueColor, t);
            channelCol = finalBackgroundColor;
            gl_Position = vec4(inPos, 0.0, 1.0);
            break;
        
        case 1:
            gl_Position = vec4(inPos.x - sunPos.x, inPos.y + sunPos.y, 0.0, 1.0);
            channelCol = vec4(0.5, 0.5, 0.5, 1.0);
            if (isSun) {
                float t = (sin(time) + 1.0) / 2.0; // Normalize sine wave to [0, 1]
                vec4 yellow = vec4(1.0, 1.0, 0.0, 1.0); // Yellow color
                vec4 orange = vec4(1.0, 0.5, 0.0, 1.0); // Orange color
                channelCol = mix(yellow, orange, t); // Blend yellow and orange
            }
            break;

        case 2:
            vec4 lightGreen = vec4(0.5, 1.0, 0.5, 1.0); // Light green for daytime
            vec4 darkGreen = vec4(0.0, 0.5, 0.0, 1.0);  // Dark green for nighttime

            // Calculate blend factor based on the sun's vertical position
            float ta = (sunPos.y + 1.0) / 2.0;

            // Interpolate between light green and dark green
            channelCol = mix(darkGreen, lightGreen, ta);

            gl_Position = vec4(inPos, 0.0, 1.0);
            break;

        
        case 3:
            gl_Position = vec4(inPos, 0.0, 1.0);
            texCoord = inTex;
            break;

        // Case 4: House Walls
        case 4:
            channelCol = vec4(0.6, 0.4, 0.2, 1.0); // Brown walls
            gl_Position = vec4(inPos, 0.0, 1.0);
            break;

        // Case 5: Roof
        case 5:
            channelCol = vec4(0.8, 0.2, 0.2, 1.0); // Red roof
            gl_Position = vec4(inPos, 0.0, 1.0);
            break;
        
        case 6:
            channelCol = vec4(0.0, 0.0, 0.0, 1.0); // Black line
            gl_Position = vec4(inPos, 0.0, 1.0);
            break;

        case 7:
            channelCol = vec4(0.3, 0.15, 0.0, 1.0); // Dark brown door
            gl_Position = vec4(inPos, 0.0, 1.0);
            break;

       case 8: // Windows
            vec4 yellow = vec4(1.0, 1.0, 0.0, 1.0);  // Yellow color
            vec4 orange = vec4(1.0, 0.5, 0.0, 1.0);  // Orange color
            vec4 interpolatedColor = mix(yellow, orange, windowAnimationProgress); 
            channelCol = vec4(interpolatedColor.rgb, transparency * interpolatedColor.a); // Apply transparency
            gl_Position = vec4(inPos, 0.0, 1.0);
            break;

        case 9: // Fence posts
            channelCol = vec4(0.4, 0.2, 0.1, 1.0); // Brown wood color
            gl_Position = vec4(inPos, 0.0, 1.0);
            break;

        case 10: // Fence horizontal bar
            channelCol = vec4(0.4, 0.2, 0.1, 1.0); // Brown wood color
            gl_Position = vec4(inPos, 0.0, 1.0);
            break;

        case 11: // Chimney
            channelCol = vec4(0.4, 0.2, 0.1, 1.0); // Brown chimney
            break;

        case 12: // Smoke
            gl_Position = vec4(inPos.x + smokePos.x, inPos.y + smokePos.y, 0.0, 1.0);
            channelCol = vec4(0.5, 0.5, 0.5, 1.0);
            break;
        case 13: // Dog's body
            vec2 position = vec2(inPos.x * dogDirection + dogPos.x, inPos.y + dogPos.y);
            gl_Position = vec4(position, 0.0, 1.0);
            texCoord = inTex;
            break;

       case 14: // Rendering "Z" letter
            // Position "Z" at zPos and apply oscillation
            vec2 zPosition = vec2(inPos.x + zPos.x, inPos.y + zPos.y);
            gl_Position = vec4(zPosition, 0.0, 1.0);

            // Color with fading effect (zAlpha controls opacity)
            channelCol = vec4(0.9, 0.9, 0.9, zAlpha); // Light gray with fading
            break;
       case 15: // Stickman rendering
            gl_Position = vec4(inPos, 0.0, 1.0); // Use the vertex position
            texCoord = inTex; // Pass the texture coordinates
            break;
     case 16: // Tree trunk
            vec4 brown = vec4(0.6, 0.4, 0.2, 1.0); // Brown color for the trunk
            vec4 white = vec4(1.0, 1.0, 1.0, 1.0); // White paint color

            // Check if the tree should be fully white
            if (paintProgress >= 1.0) {
                channelCol = white; // Fully white tree
            } else {
                // Determine if the current position is within the paintable range
                if (inPos.y <= -0.35 + (0.3 * paintProgress)) {
                    // Gradually increase the whiteness in the paintable range
                    float blendFactor = (paintProgress - (0.8 + inPos.y) / 10); 
                    blendFactor = clamp(blendFactor, 0.0, 1.0); // Ensure blend factor is in [0, 1]
                    channelCol = mix(brown, white, blendFactor); // Blend between brown and white
                
                } else {
                    // Render the unpainted area as brown
                    channelCol = brown;
                }

            }
            
            gl_Position = vec4(inPos, 0.0, 1.0);
            break;


      case 17: // Tree foliage
            channelCol = vec4(0.5, 1.0, 0.5, 1.0); // Green color for the foliage
            gl_Position = vec4(inPos, 0.0, 1.0);
            break;

      case 18:
            vec2 foodScaledPos = inPos * 2; // Scale the food to a small square
            gl_Position = vec4(foodPos + foodScaledPos, 0.0, 1.0); // Position food inside the house
            channelCol = vec4(1.0, 0.0, 1.0, 1.0); // Red color for food
            break;


        default:
            gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0);
            channelCol = inCol;
    }
}
