#version 330 core

// Input Attributes from Vertex Shader
in vec3 worldSpace_pos;
in vec3 worldSpace_norm;
in vec2 tex_coord;
in vec4 vertColor;

// Object Material Data
uniform int colorSource; // 0 = solid color (objColor), 1 = texture color (objTexture), 2 = per-vertex color (vertColor)
uniform vec4 objColor;
uniform sampler2D objTexture;
uniform float shininess;

// Camera Uniform
uniform vec3 worldSpace_camPos;

// Global Lighting Coefficients
uniform vec3 coeffs; // vec3(ka, kd, ks)

// Light Data
uniform int lightType[16]; // 0 = point light, 1 = directional light
uniform vec3 lightColor[16];
uniform vec3 lightFunction[16]; // Attenuation coefficients (for point lights)
uniform vec3 worldSpace_lightPos[16]; // Light Positions (for point lights)
uniform vec3 worldSpace_lightDir[16]; // Light Directions (for directional lights)
uniform int numLights; // Number of active lights (Max = 16)

// Output Fragment Color
out vec4 fragColor;

// Function to get the direction to the light based on its type
vec3 getToLight(int lightIndex) {
    const int LIGHT_POINT = 0;
    const int LIGHT_DIRECTIONAL = 1;

    if (lightType[lightIndex] == LIGHT_POINT) {
        return normalize(worldSpace_lightPos[lightIndex] - worldSpace_pos);
    }
    else if (lightType[lightIndex] == LIGHT_DIRECTIONAL) {
        return normalize(-worldSpace_lightDir[lightIndex]);
    }

    // Default direction if light type is undefined
    return vec3(0);
}

// Function to calculate attenuation for point lights
float attenuationFactor(int lightIndex) {
    const int LIGHT_POINT = 0;

    if (lightType[lightIndex] == LIGHT_POINT) {
        vec3 attenuation = lightFunction[lightIndex];
        float distance = length(worldSpace_lightPos[lightIndex] - worldSpace_pos);
        return 1.0 / (attenuation.x + attenuation.y * distance + attenuation.z * distance * distance);
    }

    // No attenuation for directional lights
    return 1.0;
}

// Function to compute diffuse intensity
float computeDiffuseIntensity(vec3 worldSpace_toLight) {
    return max(dot(worldSpace_toLight, normalize(worldSpace_norm)), 0.0);
}

// Corrected Function to compute specular intensity
float computeSpecularIntensity(vec3 worldSpace_toLight, vec3 worldSpace_toEye) {
    // Guard against pow(0, shininess) which is undefined
    if (shininess <= 0.0) {
        return 0.0;
    }

    // Calculate the reflection direction of the light
    vec3 worldSpace_toLightReflected = reflect(-worldSpace_toLight, normalize(worldSpace_norm));

    // Compute the dot product between the reflected light direction and the view direction
    float specAngle = max(dot(normalize(worldSpace_toEye), normalize(worldSpace_toLightReflected)), 0.0);

    // Calculate the specular intensity using the shininess exponent
    return pow(specAngle, shininess);
}

void main() {
    // Initialize ambient, diffuse, and specular components
    vec3 ambient = vec3(coeffs.x); // ka
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    // Compute the view direction vector
    vec3 worldSpace_toEye = normalize(worldSpace_camPos - worldSpace_pos);

    // Iterate over each light to accumulate diffuse and specular contributions
    for(int i = 0; i < numLights; i++) {
        // Get the direction vector to the current light
        vec3 worldSpace_toLight = getToLight(i);

        // Calculate diffuse and specular intensities
        float diffuse_intensity = computeDiffuseIntensity(worldSpace_toLight);
        float specular_intensity = computeSpecularIntensity(worldSpace_toLight, worldSpace_toEye);

        // Calculate attenuation
        float att = attenuationFactor(i);

        // Accumulate the diffuse and specular components
        diffuse += diffuse_intensity * lightColor[i] * att;
        specular += specular_intensity * lightColor[i] * att;
    }

    // Apply the global lighting coefficients
    diffuse *= coeffs.y;  // kd
    specular *= coeffs.z; // ks

    // Combine the ambient, diffuse, and specular components and clamp the result
    vec3 lighting = clamp(ambient + diffuse + specular, 0.0, 1.0);
    vec4 tempColor = vec4(lighting, 1.0);

    // Apply the appropriate object color based on the colorSource uniform
    if (colorSource == 0) {
        // Solid color
        fragColor = vec4(tempColor.rgb * objColor.rgb, tempColor.a * objColor.a);
    }
    else if (colorSource == 1){
        // Texture color
        vec4 texColor = texture(objTexture, tex_coord);
        fragColor = vec4(tempColor.rgb * texColor.rgb, tempColor.a * texColor.a);
    }
    else if (colorSource == 2) {
        // Per-vertex color
        fragColor = vec4(tempColor.rgb * vertColor.rgb, tempColor.a * vertColor.a);
    }
    else{
        // Fallback to lighting only if colorSource is undefined
        fragColor = vec4(lighting, 1.0);
    }
}
