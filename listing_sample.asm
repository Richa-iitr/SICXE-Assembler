Line	Address	Label	OPCODE	OPERAND	ObjectCode	Comment
5	00000	0	SUM	START	0		
10	00000	0	FIRST	LDX	#0	050000	
15	00003	0		LDA	#0	010000	
20	00006	0		+LDB	#0	69100000	
25	0000A	0		+LDB	#TABLE2	69101794	
30	0000E	0		BASE	TABLE2		
35	0000E	0	LOOP	ADD	TABLE,X	1BA013	
40	00011	0		ADD	TABLE2,X	1BC000	
45	00014	0		TIX	COUNT	2F200A	
50	00017	0		JLT	LOOP	3B2FF4	
55	0001A	0		+STA	TOTAL	0F102F04	
60	0001E	0		RSUB	 	4F0000	
65	00021	0	COUNT	RESW	1		
70	00024	0	TABLE	RESW	2000		
75	01794	0	TABLE2	RESW	2000		
80	02F04	0	TOTAL	RESW	2		