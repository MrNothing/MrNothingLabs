uniform sampler2D texture1; //snow
uniform sampler2D texture2; //grass
uniform sampler2D texture3; //dirt
uniform sampler2D texture4; //rocks

varying vec2 vUv;

float rangeFactor(float t, float _point, float _range){
	return max(0, 1-(abs (_point - t) / _range));
}

void main( void ) {

	vec2 position = -1.0 + 2.0 * vUv;

	vec4 tex1 = texture2D( texture1, vUv )*rangeFactor(gl_Color.r, 0, 0.33);
	vec4 tex2 = texture2D( texture2, vUv )*rangeFactor(gl_Color.g, 0.33, 0.33);
	vec4 tex3 = texture2D( texture3, vUv )*rangeFactor(gl_Color.b, 0.66, 0.33);

	vec4 color = tex1+tex2+tex3;
	
	gl_FragColor = color;
}
