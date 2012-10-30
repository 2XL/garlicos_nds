@;==============================================================================
@;
@;	"garlic_vector.s":	vector de direcciones de funciones del API de GARLIC
@;
@;==============================================================================

.section .vectors,"a",%progbits

APIVector:						@; Vector de direcciones de funciones
	.word	_gi_random			@; (código de funciones en "garlic_itcm_api.s")
	.word	_gi_divmod
	.word	_gi_num2str
	.word	_gi_print
	.word	_gi_printchar
	.word	_gi_printmat
	.word	_gi_delay
	.word	_gi_clear

.end