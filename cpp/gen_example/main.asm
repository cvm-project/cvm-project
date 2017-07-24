
main:     file format elf64-x86-64


Disassembly of section .interp:

0000000000400200 <.interp>:
  400200:	2f                   	(bad)  
  400201:	6c                   	insb   (%dx),%es:(%rdi)
  400202:	69 62 36 34 2f 6c 64 	imul   $0x646c2f34,0x36(%rdx),%esp
  400209:	2d 6c 69 6e 75       	sub    $0x756e696c,%eax
  40020e:	78 2d                	js     40023d <_init-0x99b>
  400210:	78 38                	js     40024a <_init-0x98e>
  400212:	36 2d 36 34 2e 73    	ss sub $0x732e3436,%eax
  400218:	6f                   	outsl  %ds:(%rsi),(%dx)
  400219:	2e 32 00             	xor    %cs:(%rax),%al

Disassembly of section .note.ABI-tag:

000000000040021c <.note.ABI-tag>:
  40021c:	04 00                	add    $0x0,%al
  40021e:	00 00                	add    %al,(%rax)
  400220:	10 00                	adc    %al,(%rax)
  400222:	00 00                	add    %al,(%rax)
  400224:	01 00                	add    %eax,(%rax)
  400226:	00 00                	add    %al,(%rax)
  400228:	47                   	rex.RXB
  400229:	4e 55                	rex.WRX push %rbp
  40022b:	00 00                	add    %al,(%rax)
  40022d:	00 00                	add    %al,(%rax)
  40022f:	00 02                	add    %al,(%rdx)
  400231:	00 00                	add    %al,(%rax)
  400233:	00 06                	add    %al,(%rsi)
  400235:	00 00                	add    %al,(%rax)
  400237:	00 20                	add    %ah,(%rax)
  400239:	00 00                	add    %al,(%rax)
	...

Disassembly of section .hash:

0000000000400240 <.hash>:
  400240:	11 00                	adc    %eax,(%rax)
  400242:	00 00                	add    %al,(%rax)
  400244:	1b 00                	sbb    (%rax),%eax
  400246:	00 00                	add    %al,(%rax)
  400248:	0f 00 00             	sldt   (%rax)
  40024b:	00 13                	add    %dl,(%rbx)
  40024d:	00 00                	add    %al,(%rax)
  40024f:	00 00                	add    %al,(%rax)
  400251:	00 00                	add    %al,(%rax)
  400253:	00 19                	add    %bl,(%rcx)
  400255:	00 00                	add    %al,(%rax)
  400257:	00 16                	add    %dl,(%rsi)
  400259:	00 00                	add    %al,(%rax)
  40025b:	00 10                	add    %dl,(%rax)
  40025d:	00 00                	add    %al,(%rax)
  40025f:	00 18                	add    %bl,(%rax)
  400261:	00 00                	add    %al,(%rax)
  400263:	00 08                	add    %cl,(%rax)
  400265:	00 00                	add    %al,(%rax)
  400267:	00 1a                	add    %bl,(%rdx)
  400269:	00 00                	add    %al,(%rax)
  40026b:	00 14 00             	add    %dl,(%rax,%rax,1)
  40026e:	00 00                	add    %al,(%rax)
  400270:	0c 00                	or     $0x0,%al
  400272:	00 00                	add    %al,(%rax)
  400274:	12 00                	adc    (%rax),%al
  400276:	00 00                	add    %al,(%rax)
  400278:	0b 00                	or     (%rax),%eax
  40027a:	00 00                	add    %al,(%rax)
  40027c:	0a 00                	or     (%rax),%al
  40027e:	00 00                	add    %al,(%rax)
  400280:	09 00                	or     %eax,(%rax)
  400282:	00 00                	add    %al,(%rax)
  400284:	02 00                	add    (%rax),%al
  400286:	00 00                	add    %al,(%rax)
  400288:	01 00                	add    %eax,(%rax)
	...
  4002aa:	00 00                	add    %al,(%rax)
  4002ac:	04 00                	add    $0x0,%al
	...
  4002b6:	00 00                	add    %al,(%rax)
  4002b8:	07                   	(bad)  
  4002b9:	00 00                	add    %al,(%rax)
  4002bb:	00 00                	add    %al,(%rax)
  4002bd:	00 00                	add    %al,(%rax)
  4002bf:	00 03                	add    %al,(%rbx)
  4002c1:	00 00                	add    %al,(%rax)
  4002c3:	00 00                	add    %al,(%rax)
  4002c5:	00 00                	add    %al,(%rax)
  4002c7:	00 06                	add    %al,(%rsi)
	...
  4002d1:	00 00                	add    %al,(%rax)
  4002d3:	00 11                	add    %dl,(%rcx)
	...
  4002dd:	00 00                	add    %al,(%rax)
  4002df:	00 0d 00 00 00 00    	add    %cl,0x0(%rip)        # 4002e5 <_init-0x8f3>
  4002e5:	00 00                	add    %al,(%rax)
  4002e7:	00 15 00 00 00 0e    	add    %dl,0xe000000(%rip)        # e4002ed <_end+0xddfde15>
  4002ed:	00 00                	add    %al,(%rax)
  4002ef:	00 17                	add    %dl,(%rdi)
  4002f1:	00 00                	add    %al,(%rax)
  4002f3:	00                   	.byte 0x0
  4002f4:	05                   	.byte 0x5
  4002f5:	00 00                	add    %al,(%rax)
	...

Disassembly of section .dynsym:

00000000004002f8 <.dynsym>:
	...
  400310:	0f 02 00             	lar    (%rax),%eax
  400313:	00 12                	add    %dl,(%rdx)
	...
  400325:	00 00                	add    %al,(%rax)
  400327:	00 10                	add    %dl,(%rax)
  400329:	00 00                	add    %al,(%rax)
  40032b:	00 20                	add    %ah,(%rax)
	...
  40033d:	00 00                	add    %al,(%rax)
  40033f:	00 1f                	add    %bl,(%rdi)
  400341:	00 00                	add    %al,(%rax)
  400343:	00 20                	add    %ah,(%rax)
	...
  400355:	00 00                	add    %al,(%rax)
  400357:	00 99 02 00 00 12    	add    %bl,0x12000002(%rcx)
	...
  40036d:	00 00                	add    %al,(%rax)
  40036f:	00 93 00 00 00 12    	add    %dl,0x12000000(%rbx)
	...
  400385:	00 00                	add    %al,(%rax)
  400387:	00 13                	add    %dl,(%rbx)
  400389:	01 00                	add    %eax,(%rax)
  40038b:	00 12                	add    %dl,(%rdx)
	...
  40039d:	00 00                	add    %al,(%rax)
  40039f:	00 91 01 00 00 12    	add    %dl,0x12000001(%rcx)
	...
  4003b5:	00 00                	add    %al,(%rax)
  4003b7:	00 81 00 00 00 12    	add    %al,0x12000000(%rcx)
	...
  4003cd:	00 00                	add    %al,(%rax)
  4003cf:	00 a6 02 00 00 12    	add    %ah,0x12000002(%rsi)
	...
  4003e5:	00 00                	add    %al,(%rax)
  4003e7:	00 a9 01 00 00 12    	add    %ch,0x12000001(%rcx)
  4003ed:	00 00                	add    %al,(%rax)
  4003ef:	00 80 0c 40 00 00    	add    %al,0x400c(%rax)
	...
  4003fd:	00 00                	add    %al,(%rax)
  4003ff:	00 33                	add    %dh,(%rbx)
  400401:	00 00                	add    %al,(%rax)
  400403:	00 20                	add    %ah,(%rax)
	...
  400415:	00 00                	add    %al,(%rax)
  400417:	00 57 01             	add    %dl,0x1(%rdi)
  40041a:	00 00                	add    %al,(%rax)
  40041c:	12 00                	adc    (%rax),%al
  40041e:	00 00                	add    %al,(%rax)
  400420:	90                   	nop
  400421:	0c 40                	or     $0x40,%al
	...
  40042f:	00 4f 00             	add    %cl,0x0(%rdi)
  400432:	00 00                	add    %al,(%rax)
  400434:	20 00                	and    %al,(%rax)
	...
  400446:	00 00                	add    %al,(%rax)
  400448:	4a 02 00             	rex.WX add (%rax),%al
  40044b:	00 21                	add    %ah,(%rcx)
  40044d:	00 1a                	add    %bl,(%rdx)
  40044f:	00 e0                	add    %ah,%al
  400451:	22 60 00             	and    0x0(%rax),%ah
  400454:	00 00                	add    %al,(%rax)
  400456:	00 00                	add    %al,(%rax)
  400458:	58                   	pop    %rax
  400459:	00 00                	add    %al,(%rax)
  40045b:	00 00                	add    %al,(%rax)
  40045d:	00 00                	add    %al,(%rax)
  40045f:	00 40 02             	add    %al,0x2(%rax)
  400462:	00 00                	add    %al,(%rax)
  400464:	11 00                	adc    %eax,(%rax)
  400466:	1a 00                	sbb    (%rax),%al
  400468:	40 23 60 00          	rex and 0x0(%rax),%esp
  40046c:	00 00                	add    %al,(%rax)
  40046e:	00 00                	add    %al,(%rax)
  400470:	10 01                	adc    %al,(%rcx)
  400472:	00 00                	add    %al,(%rax)
  400474:	00 00                	add    %al,(%rax)
  400476:	00 00                	add    %al,(%rax)
  400478:	c1 01 00             	roll   $0x0,(%rcx)
  40047b:	00 12                	add    %dl,(%rdx)
	...
  40048d:	00 00                	add    %al,(%rax)
  40048f:	00 cf                	add    %cl,%bh
  400491:	02 00                	add    (%rax),%al
  400493:	00 12                	add    %dl,(%rdx)
	...
  4004a5:	00 00                	add    %al,(%rax)
  4004a7:	00 c0                	add    %al,%al
  4004a9:	00 00                	add    %al,(%rax)
  4004ab:	00 12                	add    %dl,(%rdx)
	...
  4004bd:	00 00                	add    %al,(%rax)
  4004bf:	00 70 02             	add    %dh,0x2(%rax)
  4004c2:	00 00                	add    %al,(%rax)
  4004c4:	21 00                	and    %eax,(%rax)
  4004c6:	1a 00                	sbb    (%rax),%al
  4004c8:	50                   	push   %rax
  4004c9:	24 60                	and    $0x60,%al
  4004cb:	00 00                	add    %al,(%rax)
  4004cd:	00 00                	add    %al,(%rax)
  4004cf:	00 58 00             	add    %bl,0x0(%rax)
  4004d2:	00 00                	add    %al,(%rax)
  4004d4:	00 00                	add    %al,(%rax)
  4004d6:	00 00                	add    %al,(%rax)
  4004d8:	ce                   	(bad)  
  4004d9:	00 00                	add    %al,(%rax)
  4004db:	00 12                	add    %dl,(%rdx)
	...
  4004ed:	00 00                	add    %al,(%rax)
  4004ef:	00 27                	add    %ah,(%rdi)
  4004f1:	02 00                	add    (%rax),%al
  4004f3:	00 12                	add    %dl,(%rdx)
	...
  400505:	00 00                	add    %al,(%rax)
  400507:	00 b3 02 00 00 12    	add    %dh,0x12000002(%rbx)
  40050d:	00 00                	add    %al,(%rax)
  40050f:	00 f0                	add    %dh,%al
  400511:	0c 40                	or     $0x40,%al
	...
  40051f:	00 a0 02 00 00 12    	add    %ah,0x12000002(%rax)
	...
  400535:	00 00                	add    %al,(%rax)
  400537:	00 d9                	add    %bl,%cl
  400539:	02 00                	add    (%rax),%al
  40053b:	00 12                	add    %dl,(%rdx)
	...
  40054d:	00 00                	add    %al,(%rax)
  40054f:	00 b4 00 00 00 12 00 	add    %dh,0x120000(%rax,%rax,1)
	...
  400566:	00 00                	add    %al,(%rax)
  400568:	c8 02 00 00          	enterq $0x2,$0x0
  40056c:	12 00                	adc    (%rax),%al
	...

Disassembly of section .dynstr:

0000000000400580 <.dynstr>:
  400580:	00 6c 69 62          	add    %ch,0x62(%rcx,%rbp,2)
  400584:	73 74                	jae    4005fa <_init-0x5de>
  400586:	64 63 2b             	movslq %fs:(%rbx),%ebp
  400589:	2b 2e                	sub    (%rsi),%ebp
  40058b:	73 6f                	jae    4005fc <_init-0x5dc>
  40058d:	2e 36 00 5f 5f       	cs add %bl,%ss:0x5f(%rdi)
  400592:	67 6d                	insl   (%dx),%es:(%edi)
  400594:	6f                   	outsl  %ds:(%rsi),(%dx)
  400595:	6e                   	outsb  %ds:(%rsi),(%dx)
  400596:	5f                   	pop    %rdi
  400597:	73 74                	jae    40060d <_init-0x5cb>
  400599:	61                   	(bad)  
  40059a:	72 74                	jb     400610 <_init-0x5c8>
  40059c:	5f                   	pop    %rdi
  40059d:	5f                   	pop    %rdi
  40059e:	00 5f 4a             	add    %bl,0x4a(%rdi)
  4005a1:	76 5f                	jbe    400602 <_init-0x5d6>
  4005a3:	52                   	push   %rdx
  4005a4:	65 67 69 73 74 65 72 	imul   $0x6c437265,%gs:0x74(%ebx),%esi
  4005ab:	43 6c 
  4005ad:	61                   	(bad)  
  4005ae:	73 73                	jae    400623 <_init-0x5b5>
  4005b0:	65 73 00             	gs jae 4005b3 <_init-0x625>
  4005b3:	5f                   	pop    %rdi
  4005b4:	49 54                	rex.WB push %r12
  4005b6:	4d 5f                	rex.WRB pop %r15
  4005b8:	64 65 72 65          	fs gs jb 400621 <_init-0x5b7>
  4005bc:	67 69 73 74 65 72 54 	imul   $0x4d547265,0x74(%ebx),%esi
  4005c3:	4d 
  4005c4:	43 6c                	rex.XB insb (%dx),%es:(%rdi)
  4005c6:	6f                   	outsl  %ds:(%rsi),(%dx)
  4005c7:	6e                   	outsb  %ds:(%rsi),(%dx)
  4005c8:	65 54                	gs push %rsp
  4005ca:	61                   	(bad)  
  4005cb:	62                   	(bad)  
  4005cc:	6c                   	insb   (%dx),%es:(%rdi)
  4005cd:	65 00 5f 49          	add    %bl,%gs:0x49(%rdi)
  4005d1:	54                   	push   %rsp
  4005d2:	4d 5f                	rex.WRB pop %r15
  4005d4:	72 65                	jb     40063b <_init-0x59d>
  4005d6:	67 69 73 74 65 72 54 	imul   $0x4d547265,0x74(%ebx),%esi
  4005dd:	4d 
  4005de:	43 6c                	rex.XB insb (%dx),%es:(%rdi)
  4005e0:	6f                   	outsl  %ds:(%rsi),(%dx)
  4005e1:	6e                   	outsb  %ds:(%rsi),(%dx)
  4005e2:	65 54                	gs push %rsp
  4005e4:	61                   	(bad)  
  4005e5:	62                   	(bad)  
  4005e6:	6c                   	insb   (%dx),%es:(%rdi)
  4005e7:	65 00 6c 69 62       	add    %ch,%gs:0x62(%rcx,%rbp,2)
  4005ec:	67 63 63 5f          	movslq 0x5f(%ebx),%esp
  4005f0:	73 2e                	jae    400620 <_init-0x5b8>
  4005f2:	73 6f                	jae    400663 <_init-0x575>
  4005f4:	2e 31 00             	xor    %eax,%cs:(%rax)
  4005f7:	6c                   	insb   (%dx),%es:(%rdi)
  4005f8:	69 62 63 2e 73 6f 2e 	imul   $0x2e6f732e,0x63(%rdx),%esp
  4005ff:	36 00 5f 5f          	add    %bl,%ss:0x5f(%rdi)
  400603:	6c                   	insb   (%dx),%es:(%rdi)
  400604:	69 62 63 5f 73 74 61 	imul   $0x6174735f,0x63(%rdx),%esp
  40060b:	72 74                	jb     400681 <_init-0x557>
  40060d:	5f                   	pop    %rdi
  40060e:	6d                   	insl   (%dx),%es:(%rdi)
  40060f:	61                   	(bad)  
  400610:	69 6e 00 5f 5a 4e 4b 	imul   $0x4b4e5a5f,0x0(%rsi),%ebp
  400617:	53                   	push   %rbx
  400618:	74 35                	je     40064f <_init-0x589>
  40061a:	63 74 79 70          	movslq 0x70(%rcx,%rdi,2),%esi
  40061e:	65 49 63 45 31       	movslq %gs:0x31(%r13),%rax
  400623:	33 5f 4d             	xor    0x4d(%rdi),%ebx
  400626:	5f                   	pop    %rdi
  400627:	77 69                	ja     400692 <_init-0x546>
  400629:	64 65 6e             	fs outsb %gs:(%rsi),(%dx)
  40062c:	5f                   	pop    %rdi
  40062d:	69 6e 69 74 45 76 00 	imul   $0x764574,0x69(%rsi),%ebp
  400634:	5f                   	pop    %rdi
  400635:	5a                   	pop    %rdx
  400636:	4e 53                	rex.WRX push %rbx
  400638:	6f                   	outsl  %ds:(%rsi),(%dx)
  400639:	33 70 75             	xor    0x75(%rax),%esi
  40063c:	74 45                	je     400683 <_init-0x555>
  40063e:	63 00                	movslq (%rax),%eax
  400640:	5f                   	pop    %rdi
  400641:	5a                   	pop    %rdx
  400642:	4e 53                	rex.WRX push %rbx
  400644:	6f                   	outsl  %ds:(%rsi),(%dx)
  400645:	35 66 6c 75 73       	xor    $0x73756c66,%eax
  40064a:	68 45 76 00 5f       	pushq  $0x5f007645
  40064f:	5a                   	pop    %rdx
  400650:	4e 53                	rex.WRX push %rbx
  400652:	74 37                	je     40068b <_init-0x54d>
  400654:	5f                   	pop    %rdi
  400655:	5f                   	pop    %rdi
  400656:	63 78 78             	movslq 0x78(%rax),%edi
  400659:	31 31                	xor    %esi,(%rcx)
  40065b:	31 32                	xor    %esi,(%rdx)
  40065d:	62 61                	(bad)  
  40065f:	73 69                	jae    4006ca <_init-0x50e>
  400661:	63 5f 73             	movslq 0x73(%rdi),%ebx
  400664:	74 72                	je     4006d8 <_init-0x500>
  400666:	69 6e 67 49 63 53 74 	imul   $0x74536349,0x67(%rsi),%ebp
  40066d:	31 31                	xor    %esi,(%rcx)
  40066f:	63 68 61             	movslq 0x61(%rax),%ebp
  400672:	72 5f                	jb     4006d3 <_init-0x505>
  400674:	74 72                	je     4006e8 <_init-0x4f0>
  400676:	61                   	(bad)  
  400677:	69 74 73 49 63 45 53 	imul   $0x61534563,0x49(%rbx,%rsi,2),%esi
  40067e:	61 
  40067f:	49 63 45 45          	movslq 0x45(%r13),%rax
  400683:	39 5f 4d             	cmp    %ebx,0x4d(%rdi)
  400686:	5f                   	pop    %rdi
  400687:	61                   	(bad)  
  400688:	70 70                	jo     4006fa <_init-0x4de>
  40068a:	65 6e                	outsb  %gs:(%rsi),(%dx)
  40068c:	64 45 50             	fs rex.RB push %r8
  40068f:	4b 63 6d 00          	rex.WXB movslq 0x0(%r13),%rbp
  400693:	5f                   	pop    %rdi
  400694:	5a                   	pop    %rdx
  400695:	4e 53                	rex.WRX push %rbx
  400697:	74 37                	je     4006d0 <_init-0x508>
  400699:	5f                   	pop    %rdi
  40069a:	5f                   	pop    %rdi
  40069b:	63 78 78             	movslq 0x78(%rax),%edi
  40069e:	31 31                	xor    %esi,(%rcx)
  4006a0:	31 32                	xor    %esi,(%rdx)
  4006a2:	62 61                	(bad)  
  4006a4:	73 69                	jae    40070f <_init-0x4c9>
  4006a6:	63 5f 73             	movslq 0x73(%rdi),%ebx
  4006a9:	74 72                	je     40071d <_init-0x4bb>
  4006ab:	69 6e 67 49 63 53 74 	imul   $0x74536349,0x67(%rsi),%ebp
  4006b2:	31 31                	xor    %esi,(%rcx)
  4006b4:	63 68 61             	movslq 0x61(%rax),%ebp
  4006b7:	72 5f                	jb     400718 <_init-0x4c0>
  4006b9:	74 72                	je     40072d <_init-0x4ab>
  4006bb:	61                   	(bad)  
  4006bc:	69 74 73 49 63 45 53 	imul   $0x61534563,0x49(%rbx,%rsi,2),%esi
  4006c3:	61 
  4006c4:	49 63 45 45          	movslq 0x45(%r13),%rax
  4006c8:	39 5f 4d             	cmp    %ebx,0x4d(%rdi)
  4006cb:	5f                   	pop    %rdi
  4006cc:	63 72 65             	movslq 0x65(%rdx),%esi
  4006cf:	61                   	(bad)  
  4006d0:	74 65                	je     400737 <_init-0x4a1>
  4006d2:	45 52                	rex.RB push %r10
  4006d4:	6d                   	insl   (%dx),%es:(%rdi)
  4006d5:	6d                   	insl   (%dx),%es:(%rdi)
  4006d6:	00 5f 5a             	add    %bl,0x5a(%rdi)
  4006d9:	4e 53                	rex.WRX push %rbx
  4006db:	74 37                	je     400714 <_init-0x4c4>
  4006dd:	5f                   	pop    %rdi
  4006de:	5f                   	pop    %rdi
  4006df:	63 78 78             	movslq 0x78(%rax),%edi
  4006e2:	31 31                	xor    %esi,(%rcx)
  4006e4:	31 32                	xor    %esi,(%rdx)
  4006e6:	62 61                	(bad)  
  4006e8:	73 69                	jae    400753 <_init-0x485>
  4006ea:	63 5f 73             	movslq 0x73(%rdi),%ebx
  4006ed:	74 72                	je     400761 <_init-0x477>
  4006ef:	69 6e 67 49 63 53 74 	imul   $0x74536349,0x67(%rsi),%ebp
  4006f6:	31 31                	xor    %esi,(%rcx)
  4006f8:	63 68 61             	movslq 0x61(%rax),%ebp
  4006fb:	72 5f                	jb     40075c <_init-0x47c>
  4006fd:	74 72                	je     400771 <_init-0x467>
  4006ff:	61                   	(bad)  
  400700:	69 74 73 49 63 45 53 	imul   $0x61534563,0x49(%rbx,%rsi,2),%esi
  400707:	61 
  400708:	49 63 45 45          	movslq 0x45(%r13),%rax
  40070c:	44 32 45 76          	xor    0x76(%rbp),%r8b
  400710:	00 5f 5a             	add    %bl,0x5a(%rdi)
  400713:	4e 53                	rex.WRX push %rbx
  400715:	74 38                	je     40074f <_init-0x489>
  400717:	69 6f 73 5f 62 61 73 	imul   $0x7361625f,0x73(%rdi),%ebp
  40071e:	65 34 49             	gs xor $0x49,%al
  400721:	6e                   	outsb  %ds:(%rsi),(%dx)
  400722:	69 74 43 31 45 76 00 	imul   $0x5f007645,0x31(%rbx,%rax,2),%esi
  400729:	5f 
  40072a:	5a                   	pop    %rdx
  40072b:	4e 53                	rex.WRX push %rbx
  40072d:	74 38                	je     400767 <_init-0x471>
  40072f:	69 6f 73 5f 62 61 73 	imul   $0x7361625f,0x73(%rdi),%ebp
  400736:	65 34 49             	gs xor $0x49,%al
  400739:	6e                   	outsb  %ds:(%rsi),(%dx)
  40073a:	69 74 44 31 45 76 00 	imul   $0x5f007645,0x31(%rsp,%rax,2),%esi
  400741:	5f 
  400742:	5a                   	pop    %rdx
  400743:	53                   	push   %rbx
  400744:	74 31                	je     400777 <_init-0x461>
  400746:	36 5f                	ss pop %rdi
  400748:	5f                   	pop    %rdi
  400749:	6f                   	outsl  %ds:(%rsi),(%dx)
  40074a:	73 74                	jae    4007c0 <_init-0x418>
  40074c:	72 65                	jb     4007b3 <_init-0x425>
  40074e:	61                   	(bad)  
  40074f:	6d                   	insl   (%dx),%es:(%rdi)
  400750:	5f                   	pop    %rdi
  400751:	69 6e 73 65 72 74 49 	imul   $0x49747265,0x73(%rsi),%ebp
  400758:	63 53 74             	movslq 0x74(%rbx),%edx
  40075b:	31 31                	xor    %esi,(%rcx)
  40075d:	63 68 61             	movslq 0x61(%rax),%ebp
  400760:	72 5f                	jb     4007c1 <_init-0x417>
  400762:	74 72                	je     4007d6 <_init-0x402>
  400764:	61                   	(bad)  
  400765:	69 74 73 49 63 45 45 	imul   $0x52454563,0x49(%rbx,%rsi,2),%esi
  40076c:	52 
  40076d:	53                   	push   %rbx
  40076e:	74 31                	je     4007a1 <_init-0x437>
  400770:	33 62 61             	xor    0x61(%rdx),%esp
  400773:	73 69                	jae    4007de <_init-0x3fa>
  400775:	63 5f 6f             	movslq 0x6f(%rdi),%ebx
  400778:	73 74                	jae    4007ee <_init-0x3ea>
  40077a:	72 65                	jb     4007e1 <_init-0x3f7>
  40077c:	61                   	(bad)  
  40077d:	6d                   	insl   (%dx),%es:(%rdi)
  40077e:	49 54                	rex.WB push %r12
  400780:	5f                   	pop    %rdi
  400781:	54                   	push   %rsp
  400782:	30 5f 45             	xor    %bl,0x45(%rdi)
  400785:	53                   	push   %rbx
  400786:	36 5f                	ss pop %rdi
  400788:	50                   	push   %rax
  400789:	4b 53                	rex.WXB push %r11
  40078b:	33 5f 6c             	xor    0x6c(%rdi),%ebx
  40078e:	00 5f 5a             	add    %bl,0x5a(%rdi)
  400791:	53                   	push   %rbx
  400792:	74 31                	je     4007c5 <_init-0x413>
  400794:	36 5f                	ss pop %rdi
  400796:	5f                   	pop    %rdi
  400797:	74 68                	je     400801 <_init-0x3d7>
  400799:	72 6f                	jb     40080a <_init-0x3ce>
  40079b:	77 5f                	ja     4007fc <_init-0x3dc>
  40079d:	62 61 64 5f 63       	(bad)  {%k7}
  4007a2:	61                   	(bad)  
  4007a3:	73 74                	jae    400819 <_init-0x3bf>
  4007a5:	76 00                	jbe    4007a7 <_init-0x431>
  4007a7:	5f                   	pop    %rdi
  4007a8:	5a                   	pop    %rdx
  4007a9:	53                   	push   %rbx
  4007aa:	74 31                	je     4007dd <_init-0x3fb>
  4007ac:	37                   	(bad)  
  4007ad:	5f                   	pop    %rdi
  4007ae:	5f                   	pop    %rdi
  4007af:	74 68                	je     400819 <_init-0x3bf>
  4007b1:	72 6f                	jb     400822 <_init-0x3b6>
  4007b3:	77 5f                	ja     400814 <_init-0x3c4>
  4007b5:	62 61 64 5f 61       	(bad)  {%k7}
  4007ba:	6c                   	insb   (%dx),%es:(%rdi)
  4007bb:	6c                   	insb   (%dx),%es:(%rdi)
  4007bc:	6f                   	outsl  %ds:(%rsi),(%dx)
  4007bd:	63 76 00             	movslq 0x0(%rsi),%esi
  4007c0:	5f                   	pop    %rdi
  4007c1:	5a                   	pop    %rdx
  4007c2:	53                   	push   %rbx
  4007c3:	74 34                	je     4007f9 <_init-0x3df>
  4007c5:	63 6f 75             	movslq 0x75(%rdi),%ebp
  4007c8:	74 00                	je     4007ca <_init-0x40e>
  4007ca:	5f                   	pop    %rdi
  4007cb:	5a                   	pop    %rdx
  4007cc:	54                   	push   %rsp
  4007cd:	56                   	push   %rsi
  4007ce:	4e 31 30             	rex.WRX xor %r14,(%rax)
  4007d1:	5f                   	pop    %rdi
  4007d2:	5f                   	pop    %rdi
  4007d3:	63 78 78             	movslq 0x78(%rax),%edi
  4007d6:	61                   	(bad)  
  4007d7:	62                   	(bad)  {%k7}
  4007d8:	69 76 31 31 37 5f 5f 	imul   $0x5f5f3731,0x31(%rsi),%esi
  4007df:	63 6c 61 73          	movslq 0x73(%rcx,%riz,2),%ebp
  4007e3:	73 5f                	jae    400844 <_init-0x394>
  4007e5:	74 79                	je     400860 <_init-0x378>
  4007e7:	70 65                	jo     40084e <_init-0x38a>
  4007e9:	5f                   	pop    %rdi
  4007ea:	69 6e 66 6f 45 00 5f 	imul   $0x5f00456f,0x66(%rsi),%ebp
  4007f1:	5a                   	pop    %rdx
  4007f2:	54                   	push   %rsp
  4007f3:	56                   	push   %rsi
  4007f4:	4e 31 30             	rex.WRX xor %r14,(%rax)
  4007f7:	5f                   	pop    %rdi
  4007f8:	5f                   	pop    %rdi
  4007f9:	63 78 78             	movslq 0x78(%rax),%edi
  4007fc:	61                   	(bad)  
  4007fd:	62                   	(bad)  {%k7}
  4007fe:	69 76 31 32 30 5f 5f 	imul   $0x5f5f3032,0x31(%rsi),%esi
  400805:	73 69                	jae    400870 <_init-0x368>
  400807:	5f                   	pop    %rdi
  400808:	63 6c 61 73          	movslq 0x73(%rcx,%riz,2),%ebp
  40080c:	73 5f                	jae    40086d <_init-0x36b>
  40080e:	74 79                	je     400889 <_init-0x34f>
  400810:	70 65                	jo     400877 <_init-0x361>
  400812:	5f                   	pop    %rdi
  400813:	69 6e 66 6f 45 00 5f 	imul   $0x5f00456f,0x66(%rsi),%ebp
  40081a:	5a                   	pop    %rdx
  40081b:	64 6c                	fs insb (%dx),%es:(%rdi)
  40081d:	50                   	push   %rax
  40081e:	76 00                	jbe    400820 <_init-0x3b8>
  400820:	5f                   	pop    %rdi
  400821:	5a                   	pop    %rdx
  400822:	6e                   	outsb  %ds:(%rsi),(%dx)
  400823:	77 6d                	ja     400892 <_init-0x346>
  400825:	00 5f 5f             	add    %bl,0x5f(%rdi)
  400828:	63 78 61             	movslq 0x61(%rax),%edi
  40082b:	5f                   	pop    %rdi
  40082c:	61                   	(bad)  
  40082d:	74 65                	je     400894 <_init-0x344>
  40082f:	78 69                	js     40089a <_init-0x33e>
  400831:	74 00                	je     400833 <_init-0x3a5>
  400833:	5f                   	pop    %rdi
  400834:	5f                   	pop    %rdi
  400835:	67 78 78             	addr32 js 4008b0 <_init-0x328>
  400838:	5f                   	pop    %rdi
  400839:	70 65                	jo     4008a0 <_init-0x338>
  40083b:	72 73                	jb     4008b0 <_init-0x328>
  40083d:	6f                   	outsl  %ds:(%rsi),(%dx)
  40083e:	6e                   	outsb  %ds:(%rsi),(%dx)
  40083f:	61                   	(bad)  
  400840:	6c                   	insb   (%dx),%es:(%rdi)
  400841:	69 74 79 5f 76 30 00 	imul   $0x6d003076,0x5f(%rcx,%rdi,2),%esi
  400848:	6d 
  400849:	65 6d                	gs insl (%dx),%es:(%rdi)
  40084b:	63 70 79             	movslq 0x79(%rax),%esi
  40084e:	00 76 73             	add    %dh,0x73(%rsi)
  400851:	6e                   	outsb  %ds:(%rsi),(%dx)
  400852:	70 72                	jo     4008c6 <_init-0x312>
  400854:	69 6e 74 66 00 5f 55 	imul   $0x555f0066,0x74(%rsi),%ebp
  40085b:	6e                   	outsb  %ds:(%rsi),(%dx)
  40085c:	77 69                	ja     4008c7 <_init-0x311>
  40085e:	6e                   	outsb  %ds:(%rsi),(%dx)
  40085f:	64 5f                	fs pop %rdi
  400861:	52                   	push   %rdx
  400862:	65 73 75             	gs jae 4008da <_init-0x2fe>
  400865:	6d                   	insl   (%dx),%es:(%rdi)
  400866:	65 00 47 43          	add    %al,%gs:0x43(%rdi)
  40086a:	43 5f                	rex.XB pop %r15
  40086c:	33 2e                	xor    (%rsi),%ebp
  40086e:	30 00                	xor    %al,(%rax)
  400870:	47                   	rex.RXB
  400871:	4c                   	rex.WR
  400872:	49                   	rex.WB
  400873:	42                   	rex.X
  400874:	43 5f                	rex.XB pop %r15
  400876:	32 2e                	xor    (%rsi),%ch
  400878:	31 34 00             	xor    %esi,(%rax,%rax,1)
  40087b:	47                   	rex.RXB
  40087c:	4c                   	rex.WR
  40087d:	49                   	rex.WB
  40087e:	42                   	rex.X
  40087f:	43 5f                	rex.XB pop %r15
  400881:	32 2e                	xor    (%rsi),%ch
  400883:	32 2e                	xor    (%rsi),%ch
  400885:	35 00 47 4c 49       	xor    $0x494c4700,%eax
  40088a:	42                   	rex.X
  40088b:	43 58                	rex.XB pop %r8
  40088d:	58                   	pop    %rax
  40088e:	5f                   	pop    %rdi
  40088f:	33 2e                	xor    (%rsi),%ebp
  400891:	34 2e                	xor    $0x2e,%al
  400893:	39 00                	cmp    %eax,(%rax)
  400895:	43 58                	rex.XB pop %r8
  400897:	58                   	pop    %rax
  400898:	41                   	rex.B
  400899:	42                   	rex.X
  40089a:	49 5f                	rex.WB pop %r15
  40089c:	31 2e                	xor    %ebp,(%rsi)
  40089e:	33 00                	xor    (%rax),%eax
  4008a0:	47                   	rex.RXB
  4008a1:	4c                   	rex.WR
  4008a2:	49                   	rex.WB
  4008a3:	42                   	rex.X
  4008a4:	43 58                	rex.XB pop %r8
  4008a6:	58                   	pop    %rax
  4008a7:	5f                   	pop    %rdi
  4008a8:	33 2e                	xor    (%rsi),%ebp
  4008aa:	34 2e                	xor    $0x2e,%al
  4008ac:	32 31                	xor    (%rcx),%dh
  4008ae:	00 47 4c             	add    %al,0x4c(%rdi)
  4008b1:	49                   	rex.WB
  4008b2:	42                   	rex.X
  4008b3:	43 58                	rex.XB pop %r8
  4008b5:	58                   	pop    %rax
  4008b6:	5f                   	pop    %rdi
  4008b7:	33 2e                	xor    (%rsi),%ebp
  4008b9:	34 2e                	xor    $0x2e,%al
  4008bb:	31 31                	xor    %esi,(%rcx)
  4008bd:	00 47 4c             	add    %al,0x4c(%rdi)
  4008c0:	49                   	rex.WB
  4008c1:	42                   	rex.X
  4008c2:	43 58                	rex.XB pop %r8
  4008c4:	58                   	pop    %rax
  4008c5:	5f                   	pop    %rdi
  4008c6:	33 2e                	xor    (%rsi),%ebp
  4008c8:	34 00                	xor    $0x0,%al

Disassembly of section .gnu.version:

00000000004008ca <.gnu.version>:
  4008ca:	00 00                	add    %al,(%rax)
  4008cc:	02 00                	add    (%rax),%al
  4008ce:	00 00                	add    %al,(%rax)
  4008d0:	00 00                	add    %al,(%rax)
  4008d2:	02 00                	add    (%rax),%al
  4008d4:	03 00                	add    (%rax),%eax
  4008d6:	04 00                	add    $0x0,%al
  4008d8:	02 00                	add    (%rax),%al
  4008da:	05 00 05 00 02       	add    $0x2000500,%eax
  4008df:	00 00                	add    %al,(%rax)
  4008e1:	00 04 00             	add    %al,(%rax,%rax,1)
  4008e4:	00 00                	add    %al,(%rax)
  4008e6:	06                   	(bad)  
  4008e7:	00 02                	add    %al,(%rdx)
  4008e9:	00 07                	add    %al,(%rdi)
  4008eb:	00 05 00 02 00 06    	add    %al,0x6000200(%rip)        # 6400af1 <_end+0x5dfe619>
  4008f1:	00 04 00             	add    %al,(%rax,%rax,1)
  4008f4:	02 00                	add    (%rax),%al
  4008f6:	06                   	(bad)  
  4008f7:	00 02                	add    %al,(%rdx)
  4008f9:	00 08                	add    %cl,(%rax)
  4008fb:	00 02                	add    %al,(%rdx)
  4008fd:	00 09                	add    %cl,(%rcx)
	...

Disassembly of section .gnu.version_r:

0000000000400900 <.gnu.version_r>:
  400900:	01 00                	add    %eax,(%rax)
  400902:	01 00                	add    %eax,(%rax)
  400904:	69 00 00 00 10 00    	imul   $0x100000,(%rax),%eax
  40090a:	00 00                	add    %al,(%rax)
  40090c:	20 00                	and    %al,(%rax)
  40090e:	00 00                	add    %al,(%rax)
  400910:	50                   	push   %rax
  400911:	26 79 0b             	es jns 40091f <_init-0x2b9>
  400914:	00 00                	add    %al,(%rax)
  400916:	08 00                	or     %al,(%rax)
  400918:	e8 02 00 00 00       	callq  40091f <_init-0x2b9>
  40091d:	00 00                	add    %al,(%rax)
  40091f:	00 01                	add    %al,(%rcx)
  400921:	00 02                	add    %al,(%rdx)
  400923:	00 77 00             	add    %dh,0x0(%rdi)
  400926:	00 00                	add    %al,(%rax)
  400928:	10 00                	adc    %al,(%rax)
  40092a:	00 00                	add    %al,(%rax)
  40092c:	30 00                	xor    %al,(%rax)
  40092e:	00 00                	add    %al,(%rax)
  400930:	94                   	xchg   %eax,%esp
  400931:	91                   	xchg   %eax,%ecx
  400932:	96                   	xchg   %eax,%esi
  400933:	06                   	(bad)  
  400934:	00 00                	add    %al,(%rax)
  400936:	09 00                	or     %eax,(%rax)
  400938:	f0 02 00             	lock add (%rax),%al
  40093b:	00 10                	add    %dl,(%rax)
  40093d:	00 00                	add    %al,(%rax)
  40093f:	00 75 1a             	add    %dh,0x1a(%rbp)
  400942:	69 09 00 00 05 00    	imul   $0x50000,(%rcx),%ecx
  400948:	fb                   	sti    
  400949:	02 00                	add    (%rax),%al
  40094b:	00 00                	add    %al,(%rax)
  40094d:	00 00                	add    %al,(%rax)
  40094f:	00 01                	add    %al,(%rcx)
  400951:	00 05 00 01 00 00    	add    %al,0x100(%rip)        # 400a57 <_init-0x181>
  400957:	00 10                	add    %dl,(%rax)
  400959:	00 00                	add    %al,(%rax)
  40095b:	00 00                	add    %al,(%rax)
  40095d:	00 00                	add    %al,(%rax)
  40095f:	00 89 7f 29 02 00    	add    %cl,0x2297f(%rcx)
  400965:	00 07                	add    %al,(%rdi)
  400967:	00 07                	add    %al,(%rdi)
  400969:	03 00                	add    (%rax),%eax
  40096b:	00 10                	add    %dl,(%rax)
  40096d:	00 00                	add    %al,(%rax)
  40096f:	00 d3                	add    %dl,%bl
  400971:	af                   	scas   %es:(%rdi),%eax
  400972:	6b 05 00 00 06 00 15 	imul   $0x15,0x60000(%rip),%eax        # 460979 <GCC_except_table0+0x5eebd>
  400979:	03 00                	add    (%rax),%eax
  40097b:	00 10                	add    %dl,(%rax)
  40097d:	00 00                	add    %al,(%rax)
  40097f:	00 71 f8             	add    %dh,-0x8(%rcx)
  400982:	97                   	xchg   %eax,%edi
  400983:	02 00                	add    (%rax),%al
  400985:	00 04 00             	add    %al,(%rax,%rax,1)
  400988:	20 03                	and    %al,(%rbx)
  40098a:	00 00                	add    %al,(%rax)
  40098c:	10 00                	adc    %al,(%rax)
  40098e:	00 00                	add    %al,(%rax)
  400990:	61                   	(bad)  
  400991:	f8                   	clc    
  400992:	97                   	xchg   %eax,%edi
  400993:	02 00                	add    (%rax),%al
  400995:	00 03                	add    %al,(%rbx)
  400997:	00 2f                	add    %ch,(%rdi)
  400999:	03 00                	add    (%rax),%eax
  40099b:	00 10                	add    %dl,(%rax)
  40099d:	00 00                	add    %al,(%rax)
  40099f:	00 74 29 92          	add    %dh,-0x6e(%rcx,%rbp,1)
  4009a3:	08 00                	or     %al,(%rax)
  4009a5:	00 02                	add    %al,(%rdx)
  4009a7:	00 3e                	add    %bh,(%rsi)
  4009a9:	03 00                	add    (%rax),%eax
  4009ab:	00 00                	add    %al,(%rax)
  4009ad:	00 00                	add    %al,(%rax)
	...

Disassembly of section .rela.dyn:

00000000004009b0 <.rela.dyn>:
  4009b0:	10 22                	adc    %ah,(%rdx)
  4009b2:	60                   	(bad)  
  4009b3:	00 00                	add    %al,(%rax)
  4009b5:	00 00                	add    %al,(%rax)
  4009b7:	00 06                	add    %al,(%rsi)
  4009b9:	00 00                	add    %al,(%rax)
  4009bb:	00 02                	add    %al,(%rdx)
	...
  4009c5:	00 00                	add    %al,(%rax)
  4009c7:	00 e0                	add    %ah,%al
  4009c9:	22 60 00             	and    0x0(%rax),%ah
  4009cc:	00 00                	add    %al,(%rax)
  4009ce:	00 00                	add    %al,(%rax)
  4009d0:	05 00 00 00 0e       	add    $0xe000000,%eax
	...
  4009dd:	00 00                	add    %al,(%rax)
  4009df:	00 40 23             	add    %al,0x23(%rax)
  4009e2:	60                   	(bad)  
  4009e3:	00 00                	add    %al,(%rax)
  4009e5:	00 00                	add    %al,(%rax)
  4009e7:	00 05 00 00 00 0f    	add    %al,0xf000000(%rip)        # f4009ed <_end+0xedfe515>
	...
  4009f5:	00 00                	add    %al,(%rax)
  4009f7:	00 50 24             	add    %dl,0x24(%rax)
  4009fa:	60                   	(bad)  
  4009fb:	00 00                	add    %al,(%rax)
  4009fd:	00 00                	add    %al,(%rax)
  4009ff:	00 05 00 00 00 13    	add    %al,0x13000000(%rip)        # 13400a05 <_end+0x12dfe52d>
	...

Disassembly of section .rela.plt:

0000000000400a10 <.rela.plt>:
  400a10:	30 22                	xor    %ah,(%rdx)
  400a12:	60                   	(bad)  
  400a13:	00 00                	add    %al,(%rax)
  400a15:	00 00                	add    %al,(%rax)
  400a17:	00 07                	add    %al,(%rdi)
  400a19:	00 00                	add    %al,(%rax)
  400a1b:	00 01                	add    %al,(%rcx)
	...
  400a25:	00 00                	add    %al,(%rax)
  400a27:	00 38                	add    %bh,(%rax)
  400a29:	22 60 00             	and    0x0(%rax),%ah
  400a2c:	00 00                	add    %al,(%rax)
  400a2e:	00 00                	add    %al,(%rax)
  400a30:	07                   	(bad)  
  400a31:	00 00                	add    %al,(%rax)
  400a33:	00 04 00             	add    %al,(%rax,%rax,1)
	...
  400a3e:	00 00                	add    %al,(%rax)
  400a40:	40 22 60 00          	and    0x0(%rax),%spl
  400a44:	00 00                	add    %al,(%rax)
  400a46:	00 00                	add    %al,(%rax)
  400a48:	07                   	(bad)  
  400a49:	00 00                	add    %al,(%rax)
  400a4b:	00 05 00 00 00 00    	add    %al,0x0(%rip)        # 400a51 <_init-0x187>
  400a51:	00 00                	add    %al,(%rax)
  400a53:	00 00                	add    %al,(%rax)
  400a55:	00 00                	add    %al,(%rax)
  400a57:	00 48 22             	add    %cl,0x22(%rax)
  400a5a:	60                   	(bad)  
  400a5b:	00 00                	add    %al,(%rax)
  400a5d:	00 00                	add    %al,(%rax)
  400a5f:	00 07                	add    %al,(%rdi)
  400a61:	00 00                	add    %al,(%rax)
  400a63:	00 06                	add    %al,(%rsi)
	...
  400a6d:	00 00                	add    %al,(%rax)
  400a6f:	00 50 22             	add    %dl,0x22(%rax)
  400a72:	60                   	(bad)  
  400a73:	00 00                	add    %al,(%rax)
  400a75:	00 00                	add    %al,(%rax)
  400a77:	00 07                	add    %al,(%rdi)
  400a79:	00 00                	add    %al,(%rax)
  400a7b:	00 07                	add    %al,(%rdi)
	...
  400a85:	00 00                	add    %al,(%rax)
  400a87:	00 58 22             	add    %bl,0x22(%rax)
  400a8a:	60                   	(bad)  
  400a8b:	00 00                	add    %al,(%rax)
  400a8d:	00 00                	add    %al,(%rax)
  400a8f:	00 07                	add    %al,(%rdi)
  400a91:	00 00                	add    %al,(%rax)
  400a93:	00 08                	add    %cl,(%rax)
	...
  400a9d:	00 00                	add    %al,(%rax)
  400a9f:	00 60 22             	add    %ah,0x22(%rax)
  400aa2:	60                   	(bad)  
  400aa3:	00 00                	add    %al,(%rax)
  400aa5:	00 00                	add    %al,(%rax)
  400aa7:	00 07                	add    %al,(%rdi)
  400aa9:	00 00                	add    %al,(%rax)
  400aab:	00 09                	add    %cl,(%rcx)
	...
  400ab5:	00 00                	add    %al,(%rax)
  400ab7:	00 68 22             	add    %ch,0x22(%rax)
  400aba:	60                   	(bad)  
  400abb:	00 00                	add    %al,(%rax)
  400abd:	00 00                	add    %al,(%rax)
  400abf:	00 07                	add    %al,(%rdi)
  400ac1:	00 00                	add    %al,(%rax)
  400ac3:	00 0a                	add    %cl,(%rdx)
	...
  400acd:	00 00                	add    %al,(%rax)
  400acf:	00 70 22             	add    %dh,0x22(%rax)
  400ad2:	60                   	(bad)  
  400ad3:	00 00                	add    %al,(%rax)
  400ad5:	00 00                	add    %al,(%rax)
  400ad7:	00 07                	add    %al,(%rdi)
  400ad9:	00 00                	add    %al,(%rax)
  400adb:	00 0c 00             	add    %cl,(%rax,%rax,1)
	...
  400ae6:	00 00                	add    %al,(%rax)
  400ae8:	78 22                	js     400b0c <_init-0xcc>
  400aea:	60                   	(bad)  
  400aeb:	00 00                	add    %al,(%rax)
  400aed:	00 00                	add    %al,(%rax)
  400aef:	00 07                	add    %al,(%rdi)
  400af1:	00 00                	add    %al,(%rax)
  400af3:	00 10                	add    %dl,(%rax)
	...
  400afd:	00 00                	add    %al,(%rax)
  400aff:	00 80 22 60 00 00    	add    %al,0x6022(%rax)
  400b05:	00 00                	add    %al,(%rax)
  400b07:	00 07                	add    %al,(%rdi)
  400b09:	00 00                	add    %al,(%rax)
  400b0b:	00 11                	add    %dl,(%rcx)
	...
  400b15:	00 00                	add    %al,(%rax)
  400b17:	00 88 22 60 00 00    	add    %cl,0x6022(%rax)
  400b1d:	00 00                	add    %al,(%rax)
  400b1f:	00 07                	add    %al,(%rdi)
  400b21:	00 00                	add    %al,(%rax)
  400b23:	00 12                	add    %dl,(%rdx)
	...
  400b2d:	00 00                	add    %al,(%rax)
  400b2f:	00 90 22 60 00 00    	add    %dl,0x6022(%rax)
  400b35:	00 00                	add    %al,(%rax)
  400b37:	00 07                	add    %al,(%rdi)
  400b39:	00 00                	add    %al,(%rax)
  400b3b:	00 14 00             	add    %dl,(%rax,%rax,1)
	...
  400b46:	00 00                	add    %al,(%rax)
  400b48:	98                   	cwtl   
  400b49:	22 60 00             	and    0x0(%rax),%ah
  400b4c:	00 00                	add    %al,(%rax)
  400b4e:	00 00                	add    %al,(%rax)
  400b50:	07                   	(bad)  
  400b51:	00 00                	add    %al,(%rax)
  400b53:	00 15 00 00 00 00    	add    %dl,0x0(%rip)        # 400b59 <_init-0x7f>
  400b59:	00 00                	add    %al,(%rax)
  400b5b:	00 00                	add    %al,(%rax)
  400b5d:	00 00                	add    %al,(%rax)
  400b5f:	00 a0 22 60 00 00    	add    %ah,0x6022(%rax)
  400b65:	00 00                	add    %al,(%rax)
  400b67:	00 07                	add    %al,(%rdi)
  400b69:	00 00                	add    %al,(%rax)
  400b6b:	00 16                	add    %dl,(%rsi)
	...
  400b75:	00 00                	add    %al,(%rax)
  400b77:	00 a8 22 60 00 00    	add    %ch,0x6022(%rax)
  400b7d:	00 00                	add    %al,(%rax)
  400b7f:	00 07                	add    %al,(%rdi)
  400b81:	00 00                	add    %al,(%rax)
  400b83:	00 17                	add    %dl,(%rdi)
	...
  400b8d:	00 00                	add    %al,(%rax)
  400b8f:	00 b0 22 60 00 00    	add    %dh,0x6022(%rax)
  400b95:	00 00                	add    %al,(%rax)
  400b97:	00 07                	add    %al,(%rdi)
  400b99:	00 00                	add    %al,(%rax)
  400b9b:	00 18                	add    %bl,(%rax)
	...
  400ba5:	00 00                	add    %al,(%rax)
  400ba7:	00 b8 22 60 00 00    	add    %bh,0x6022(%rax)
  400bad:	00 00                	add    %al,(%rax)
  400baf:	00 07                	add    %al,(%rdi)
  400bb1:	00 00                	add    %al,(%rax)
  400bb3:	00 19                	add    %bl,(%rcx)
	...
  400bbd:	00 00                	add    %al,(%rax)
  400bbf:	00 c0                	add    %al,%al
  400bc1:	22 60 00             	and    0x0(%rax),%ah
  400bc4:	00 00                	add    %al,(%rax)
  400bc6:	00 00                	add    %al,(%rax)
  400bc8:	07                   	(bad)  
  400bc9:	00 00                	add    %al,(%rax)
  400bcb:	00 1a                	add    %bl,(%rdx)
	...

Disassembly of section .init:

0000000000400bd8 <_init>:
  400bd8:	48 83 ec 08          	sub    $0x8,%rsp
  400bdc:	48 8b 05 2d 16 20 00 	mov    0x20162d(%rip),%rax        # 602210 <_DYNAMIC+0x1f0>
  400be3:	48 85 c0             	test   %rax,%rax
  400be6:	74 05                	je     400bed <_init+0x15>
  400be8:	e8 53 01 00 00       	callq  400d40 <memcpy@plt+0x10>
  400bed:	48 83 c4 08          	add    $0x8,%rsp
  400bf1:	c3                   	retq   

Disassembly of section .plt:

0000000000400c00 <_ZSt16__throw_bad_castv@plt-0x10>:
  400c00:	ff 35 1a 16 20 00    	pushq  0x20161a(%rip)        # 602220 <_GLOBAL_OFFSET_TABLE_+0x8>
  400c06:	ff 25 1c 16 20 00    	jmpq   *0x20161c(%rip)        # 602228 <_GLOBAL_OFFSET_TABLE_+0x10>
  400c0c:	0f 1f 40 00          	nopl   0x0(%rax)

0000000000400c10 <_ZSt16__throw_bad_castv@plt>:
  400c10:	ff 25 1a 16 20 00    	jmpq   *0x20161a(%rip)        # 602230 <_GLOBAL_OFFSET_TABLE_+0x18>
  400c16:	68 00 00 00 00       	pushq  $0x0
  400c1b:	e9 e0 ff ff ff       	jmpq   400c00 <_init+0x28>

0000000000400c20 <_ZdlPv@plt>:
  400c20:	ff 25 12 16 20 00    	jmpq   *0x201612(%rip)        # 602238 <_GLOBAL_OFFSET_TABLE_+0x20>
  400c26:	68 01 00 00 00       	pushq  $0x1
  400c2b:	e9 d0 ff ff ff       	jmpq   400c00 <_init+0x28>

0000000000400c30 <_ZNKSt5ctypeIcE13_M_widen_initEv@plt>:
  400c30:	ff 25 0a 16 20 00    	jmpq   *0x20160a(%rip)        # 602240 <_GLOBAL_OFFSET_TABLE_+0x28>
  400c36:	68 02 00 00 00       	pushq  $0x2
  400c3b:	e9 c0 ff ff ff       	jmpq   400c00 <_init+0x28>

0000000000400c40 <_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERmm@plt>:
  400c40:	ff 25 02 16 20 00    	jmpq   *0x201602(%rip)        # 602248 <_GLOBAL_OFFSET_TABLE_+0x30>
  400c46:	68 03 00 00 00       	pushq  $0x3
  400c4b:	e9 b0 ff ff ff       	jmpq   400c00 <_init+0x28>

0000000000400c50 <_ZNSt8ios_base4InitC1Ev@plt>:
  400c50:	ff 25 fa 15 20 00    	jmpq   *0x2015fa(%rip)        # 602250 <_GLOBAL_OFFSET_TABLE_+0x38>
  400c56:	68 04 00 00 00       	pushq  $0x4
  400c5b:	e9 a0 ff ff ff       	jmpq   400c00 <_init+0x28>

0000000000400c60 <__libc_start_main@plt>:
  400c60:	ff 25 f2 15 20 00    	jmpq   *0x2015f2(%rip)        # 602258 <_GLOBAL_OFFSET_TABLE_+0x40>
  400c66:	68 05 00 00 00       	pushq  $0x5
  400c6b:	e9 90 ff ff ff       	jmpq   400c00 <_init+0x28>

0000000000400c70 <__cxa_atexit@plt>:
  400c70:	ff 25 ea 15 20 00    	jmpq   *0x2015ea(%rip)        # 602260 <_GLOBAL_OFFSET_TABLE_+0x48>
  400c76:	68 06 00 00 00       	pushq  $0x6
  400c7b:	e9 80 ff ff ff       	jmpq   400c00 <_init+0x28>

0000000000400c80 <_ZNSt8ios_base4InitD1Ev@plt>:
  400c80:	ff 25 e2 15 20 00    	jmpq   *0x2015e2(%rip)        # 602268 <_GLOBAL_OFFSET_TABLE_+0x50>
  400c86:	68 07 00 00 00       	pushq  $0x7
  400c8b:	e9 70 ff ff ff       	jmpq   400c00 <_init+0x28>

0000000000400c90 <_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev@plt>:
  400c90:	ff 25 da 15 20 00    	jmpq   *0x2015da(%rip)        # 602270 <_GLOBAL_OFFSET_TABLE_+0x58>
  400c96:	68 08 00 00 00       	pushq  $0x8
  400c9b:	e9 60 ff ff ff       	jmpq   400c00 <_init+0x28>

0000000000400ca0 <_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@plt>:
  400ca0:	ff 25 d2 15 20 00    	jmpq   *0x2015d2(%rip)        # 602278 <_GLOBAL_OFFSET_TABLE_+0x60>
  400ca6:	68 09 00 00 00       	pushq  $0x9
  400cab:	e9 50 ff ff ff       	jmpq   400c00 <_init+0x28>

0000000000400cb0 <vsnprintf@plt>:
  400cb0:	ff 25 ca 15 20 00    	jmpq   *0x2015ca(%rip)        # 602280 <_GLOBAL_OFFSET_TABLE_+0x68>
  400cb6:	68 0a 00 00 00       	pushq  $0xa
  400cbb:	e9 40 ff ff ff       	jmpq   400c00 <_init+0x28>

0000000000400cc0 <_ZNSo5flushEv@plt>:
  400cc0:	ff 25 c2 15 20 00    	jmpq   *0x2015c2(%rip)        # 602288 <_GLOBAL_OFFSET_TABLE_+0x70>
  400cc6:	68 0b 00 00 00       	pushq  $0xb
  400ccb:	e9 30 ff ff ff       	jmpq   400c00 <_init+0x28>

0000000000400cd0 <_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcm@plt>:
  400cd0:	ff 25 ba 15 20 00    	jmpq   *0x2015ba(%rip)        # 602290 <_GLOBAL_OFFSET_TABLE_+0x78>
  400cd6:	68 0c 00 00 00       	pushq  $0xc
  400cdb:	e9 20 ff ff ff       	jmpq   400c00 <_init+0x28>

0000000000400ce0 <_ZSt17__throw_bad_allocv@plt>:
  400ce0:	ff 25 b2 15 20 00    	jmpq   *0x2015b2(%rip)        # 602298 <_GLOBAL_OFFSET_TABLE_+0x80>
  400ce6:	68 0d 00 00 00       	pushq  $0xd
  400ceb:	e9 10 ff ff ff       	jmpq   400c00 <_init+0x28>

0000000000400cf0 <__gxx_personality_v0@plt>:
  400cf0:	ff 25 aa 15 20 00    	jmpq   *0x2015aa(%rip)        # 6022a0 <_GLOBAL_OFFSET_TABLE_+0x88>
  400cf6:	68 0e 00 00 00       	pushq  $0xe
  400cfb:	e9 00 ff ff ff       	jmpq   400c00 <_init+0x28>

0000000000400d00 <_Znwm@plt>:
  400d00:	ff 25 a2 15 20 00    	jmpq   *0x2015a2(%rip)        # 6022a8 <_GLOBAL_OFFSET_TABLE_+0x90>
  400d06:	68 0f 00 00 00       	pushq  $0xf
  400d0b:	e9 f0 fe ff ff       	jmpq   400c00 <_init+0x28>

0000000000400d10 <_Unwind_Resume@plt>:
  400d10:	ff 25 9a 15 20 00    	jmpq   *0x20159a(%rip)        # 6022b0 <_GLOBAL_OFFSET_TABLE_+0x98>
  400d16:	68 10 00 00 00       	pushq  $0x10
  400d1b:	e9 e0 fe ff ff       	jmpq   400c00 <_init+0x28>

0000000000400d20 <_ZNSo3putEc@plt>:
  400d20:	ff 25 92 15 20 00    	jmpq   *0x201592(%rip)        # 6022b8 <_GLOBAL_OFFSET_TABLE_+0xa0>
  400d26:	68 11 00 00 00       	pushq  $0x11
  400d2b:	e9 d0 fe ff ff       	jmpq   400c00 <_init+0x28>

0000000000400d30 <memcpy@plt>:
  400d30:	ff 25 8a 15 20 00    	jmpq   *0x20158a(%rip)        # 6022c0 <_GLOBAL_OFFSET_TABLE_+0xa8>
  400d36:	68 12 00 00 00       	pushq  $0x12
  400d3b:	e9 c0 fe ff ff       	jmpq   400c00 <_init+0x28>

Disassembly of section .plt.got:

0000000000400d40 <.plt.got>:
  400d40:	ff 25 ca 14 20 00    	jmpq   *0x2014ca(%rip)        # 602210 <_DYNAMIC+0x1f0>
  400d46:	66 90                	xchg   %ax,%ax

Disassembly of section .text:

0000000000400d50 <_GLOBAL__sub_I_execute.cpp>:
  400d50:	50                   	push   %rax
  400d51:	bf d0 24 60 00       	mov    $0x6024d0,%edi
  400d56:	e8 f5 fe ff ff       	callq  400c50 <_ZNSt8ios_base4InitC1Ev@plt>
  400d5b:	bf 80 0c 40 00       	mov    $0x400c80,%edi
  400d60:	be d0 24 60 00       	mov    $0x6024d0,%esi
  400d65:	ba d0 22 60 00       	mov    $0x6022d0,%edx
  400d6a:	e8 01 ff ff ff       	callq  400c70 <__cxa_atexit@plt>
  400d6f:	48 c7 05 36 17 20 00 	movq   $0x6024c0,0x201736(%rip)        # 6024b0 <_ZL9SEPARATOR>
  400d76:	c0 24 60 00 
  400d7a:	66 c7 05 3d 17 20 00 	movw   $0x202c,0x20173d(%rip)        # 6024c0 <_ZL9SEPARATOR+0x10>
  400d81:	2c 20 
  400d83:	48 c7 05 2a 17 20 00 	movq   $0x2,0x20172a(%rip)        # 6024b8 <_ZL9SEPARATOR+0x8>
  400d8a:	02 00 00 00 
  400d8e:	c6 05 2d 17 20 00 00 	movb   $0x0,0x20172d(%rip)        # 6024c2 <_ZL9SEPARATOR+0x12>
  400d95:	bf 90 0c 40 00       	mov    $0x400c90,%edi
  400d9a:	be b0 24 60 00       	mov    $0x6024b0,%esi
  400d9f:	ba d0 22 60 00       	mov    $0x6022d0,%edx
  400da4:	58                   	pop    %rax
  400da5:	e9 c6 fe ff ff       	jmpq   400c70 <__cxa_atexit@plt>
  400daa:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)

0000000000400db0 <_start>:
  400db0:	31 ed                	xor    %ebp,%ebp
  400db2:	49 89 d1             	mov    %rdx,%r9
  400db5:	5e                   	pop    %rsi
  400db6:	48 89 e2             	mov    %rsp,%rdx
  400db9:	48 83 e4 f0          	and    $0xfffffffffffffff0,%rsp
  400dbd:	50                   	push   %rax
  400dbe:	54                   	push   %rsp
  400dbf:	49 c7 c0 a0 17 40 00 	mov    $0x4017a0,%r8
  400dc6:	48 c7 c1 30 17 40 00 	mov    $0x401730,%rcx
  400dcd:	48 c7 c7 b0 0e 40 00 	mov    $0x400eb0,%rdi
  400dd4:	e8 87 fe ff ff       	callq  400c60 <__libc_start_main@plt>
  400dd9:	f4                   	hlt    
  400dda:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)

0000000000400de0 <deregister_tm_clones>:
  400de0:	b8 df 22 60 00       	mov    $0x6022df,%eax
  400de5:	55                   	push   %rbp
  400de6:	48 2d d8 22 60 00    	sub    $0x6022d8,%rax
  400dec:	48 83 f8 0e          	cmp    $0xe,%rax
  400df0:	48 89 e5             	mov    %rsp,%rbp
  400df3:	76 1b                	jbe    400e10 <deregister_tm_clones+0x30>
  400df5:	b8 00 00 00 00       	mov    $0x0,%eax
  400dfa:	48 85 c0             	test   %rax,%rax
  400dfd:	74 11                	je     400e10 <deregister_tm_clones+0x30>
  400dff:	5d                   	pop    %rbp
  400e00:	bf d8 22 60 00       	mov    $0x6022d8,%edi
  400e05:	ff e0                	jmpq   *%rax
  400e07:	66 0f 1f 84 00 00 00 	nopw   0x0(%rax,%rax,1)
  400e0e:	00 00 
  400e10:	5d                   	pop    %rbp
  400e11:	c3                   	retq   
  400e12:	0f 1f 40 00          	nopl   0x0(%rax)
  400e16:	66 2e 0f 1f 84 00 00 	nopw   %cs:0x0(%rax,%rax,1)
  400e1d:	00 00 00 

0000000000400e20 <register_tm_clones>:
  400e20:	be d8 22 60 00       	mov    $0x6022d8,%esi
  400e25:	55                   	push   %rbp
  400e26:	48 81 ee d8 22 60 00 	sub    $0x6022d8,%rsi
  400e2d:	48 c1 fe 03          	sar    $0x3,%rsi
  400e31:	48 89 e5             	mov    %rsp,%rbp
  400e34:	48 89 f0             	mov    %rsi,%rax
  400e37:	48 c1 e8 3f          	shr    $0x3f,%rax
  400e3b:	48 01 c6             	add    %rax,%rsi
  400e3e:	48 d1 fe             	sar    %rsi
  400e41:	74 15                	je     400e58 <register_tm_clones+0x38>
  400e43:	b8 00 00 00 00       	mov    $0x0,%eax
  400e48:	48 85 c0             	test   %rax,%rax
  400e4b:	74 0b                	je     400e58 <register_tm_clones+0x38>
  400e4d:	5d                   	pop    %rbp
  400e4e:	bf d8 22 60 00       	mov    $0x6022d8,%edi
  400e53:	ff e0                	jmpq   *%rax
  400e55:	0f 1f 00             	nopl   (%rax)
  400e58:	5d                   	pop    %rbp
  400e59:	c3                   	retq   
  400e5a:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)

0000000000400e60 <__do_global_dtors_aux>:
  400e60:	80 3d 41 16 20 00 00 	cmpb   $0x0,0x201641(%rip)        # 6024a8 <completed.7585>
  400e67:	75 11                	jne    400e7a <__do_global_dtors_aux+0x1a>
  400e69:	55                   	push   %rbp
  400e6a:	48 89 e5             	mov    %rsp,%rbp
  400e6d:	e8 6e ff ff ff       	callq  400de0 <deregister_tm_clones>
  400e72:	5d                   	pop    %rbp
  400e73:	c6 05 2e 16 20 00 01 	movb   $0x1,0x20162e(%rip)        # 6024a8 <completed.7585>
  400e7a:	f3 c3                	repz retq 
  400e7c:	0f 1f 40 00          	nopl   0x0(%rax)

0000000000400e80 <frame_dummy>:
  400e80:	bf 18 20 60 00       	mov    $0x602018,%edi
  400e85:	48 83 3f 00          	cmpq   $0x0,(%rdi)
  400e89:	75 05                	jne    400e90 <frame_dummy+0x10>
  400e8b:	eb 93                	jmp    400e20 <register_tm_clones>
  400e8d:	0f 1f 00             	nopl   (%rax)
  400e90:	b8 00 00 00 00       	mov    $0x0,%eax
  400e95:	48 85 c0             	test   %rax,%rax
  400e98:	74 f1                	je     400e8b <frame_dummy+0xb>
  400e9a:	55                   	push   %rbp
  400e9b:	48 89 e5             	mov    %rsp,%rbp
  400e9e:	ff d0                	callq  *%rax
  400ea0:	5d                   	pop    %rbp
  400ea1:	e9 7a ff ff ff       	jmpq   400e20 <register_tm_clones>
  400ea6:	66 2e 0f 1f 84 00 00 	nopw   %cs:0x0(%rax,%rax,1)
  400ead:	00 00 00 

0000000000400eb0 <main>:
  400eb0:	55                   	push   %rbp
  400eb1:	41 57                	push   %r15
  400eb3:	41 56                	push   %r14
  400eb5:	41 55                	push   %r13
  400eb7:	41 54                	push   %r12
  400eb9:	53                   	push   %rbx
  400eba:	48 81 ec 08 01 00 00 	sub    $0x108,%rsp
  400ec1:	0f 57 c0             	xorps  %xmm0,%xmm0
  400ec4:	0f 29 84 24 90 00 00 	movaps %xmm0,0x90(%rsp)
  400ecb:	00 
  400ecc:	48 c7 84 24 a0 00 00 	movq   $0x0,0xa0(%rsp)
  400ed3:	00 00 00 00 00 
  400ed8:	0f 28 05 e1 08 00 00 	movaps 0x8e1(%rip),%xmm0        # 4017c0 <_IO_stdin_used+0x10>
  400edf:	0f 29 44 24 70       	movaps %xmm0,0x70(%rsp)
  400ee4:	0f 28 05 e5 08 00 00 	movaps 0x8e5(%rip),%xmm0        # 4017d0 <_IO_stdin_used+0x20>
  400eeb:	0f 29 84 24 80 00 00 	movaps %xmm0,0x80(%rsp)
  400ef2:	00 
  400ef3:	48 8d bc 24 90 00 00 	lea    0x90(%rsp),%rdi
  400efa:	00 
  400efb:	48 8d 74 24 70       	lea    0x70(%rsp),%rsi
  400f00:	e8 ab 05 00 00       	callq  4014b0 <_ZNSt6vectorISt5tupleIJllllEESaIS1_EE19_M_emplace_back_auxIJS1_EEEvDpOT_>
  400f05:	48 8b 9c 24 98 00 00 	mov    0x98(%rsp),%rbx
  400f0c:	00 
  400f0d:	0f 28 05 cc 08 00 00 	movaps 0x8cc(%rip),%xmm0        # 4017e0 <_IO_stdin_used+0x30>
  400f14:	48 3b 9c 24 a0 00 00 	cmp    0xa0(%rsp),%rbx
  400f1b:	00 
  400f1c:	0f 29 44 24 50       	movaps %xmm0,0x50(%rsp)
  400f21:	0f 28 0d c8 08 00 00 	movaps 0x8c8(%rip),%xmm1        # 4017f0 <_IO_stdin_used+0x40>
  400f28:	0f 29 4c 24 60       	movaps %xmm1,0x60(%rsp)
  400f2d:	74 15                	je     400f44 <main+0x94>
  400f2f:	0f 11 03             	movups %xmm0,(%rbx)
  400f32:	0f 11 4b 10          	movups %xmm1,0x10(%rbx)
  400f36:	48 83 c3 20          	add    $0x20,%rbx
  400f3a:	48 89 9c 24 98 00 00 	mov    %rbx,0x98(%rsp)
  400f41:	00 
  400f42:	eb 1a                	jmp    400f5e <main+0xae>
  400f44:	48 8d bc 24 90 00 00 	lea    0x90(%rsp),%rdi
  400f4b:	00 
  400f4c:	48 8d 74 24 50       	lea    0x50(%rsp),%rsi
  400f51:	e8 5a 05 00 00       	callq  4014b0 <_ZNSt6vectorISt5tupleIJllllEESaIS1_EE19_M_emplace_back_auxIJS1_EEEvDpOT_>
  400f56:	48 8b 9c 24 98 00 00 	mov    0x98(%rsp),%rbx
  400f5d:	00 
  400f5e:	48 c7 44 24 28 10 18 	movq   $0x401810,0x28(%rsp)
  400f65:	40 00 
  400f67:	0f 57 c0             	xorps  %xmm0,%xmm0
  400f6a:	0f 11 44 24 40       	movups %xmm0,0x40(%rsp)
  400f6f:	0f 11 44 24 30       	movups %xmm0,0x30(%rsp)
  400f74:	48 8b ac 24 90 00 00 	mov    0x90(%rsp),%rbp
  400f7b:	00 
  400f7c:	49 89 dc             	mov    %rbx,%r12
  400f7f:	49 29 ec             	sub    %rbp,%r12
  400f82:	4d 89 e6             	mov    %r12,%r14
  400f85:	49 c1 fe 05          	sar    $0x5,%r14
  400f89:	74 76                	je     401001 <main+0x151>
  400f8b:	4c 89 f0             	mov    %r14,%rax
  400f8e:	48 c1 e8 3b          	shr    $0x3b,%rax
  400f92:	0f 85 c7 04 00 00    	jne    40145f <main+0x5af>
  400f98:	4c 89 e7             	mov    %r12,%rdi
  400f9b:	e8 60 fd ff ff       	callq  400d00 <_Znwm@plt>
  400fa0:	49 89 c7             	mov    %rax,%r15
  400fa3:	48 39 dd             	cmp    %rbx,%rbp
  400fa6:	0f 84 88 01 00 00    	je     401134 <main+0x284>
  400fac:	49 83 c4 e0          	add    $0xffffffffffffffe0,%r12
  400fb0:	44 89 e0             	mov    %r12d,%eax
  400fb3:	c1 e8 05             	shr    $0x5,%eax
  400fb6:	ff c0                	inc    %eax
  400fb8:	a8 07                	test   $0x7,%al
  400fba:	0f 84 93 00 00 00    	je     401053 <main+0x1a3>
  400fc0:	44 89 e1             	mov    %r12d,%ecx
  400fc3:	c1 e9 05             	shr    $0x5,%ecx
  400fc6:	ff c1                	inc    %ecx
  400fc8:	83 e1 07             	and    $0x7,%ecx
  400fcb:	48 89 ca             	mov    %rcx,%rdx
  400fce:	48 c1 e2 05          	shl    $0x5,%rdx
  400fd2:	49 8d 04 17          	lea    (%r15,%rdx,1),%rax
  400fd6:	48 f7 d9             	neg    %rcx
  400fd9:	4c 89 fe             	mov    %r15,%rsi
  400fdc:	48 89 ef             	mov    %rbp,%rdi
  400fdf:	90                   	nop
  400fe0:	0f 10 07             	movups (%rdi),%xmm0
  400fe3:	0f 10 4f 10          	movups 0x10(%rdi),%xmm1
  400fe7:	0f 11 4e 10          	movups %xmm1,0x10(%rsi)
  400feb:	0f 11 06             	movups %xmm0,(%rsi)
  400fee:	48 83 c7 20          	add    $0x20,%rdi
  400ff2:	48 83 c6 20          	add    $0x20,%rsi
  400ff6:	48 ff c1             	inc    %rcx
  400ff9:	75 e5                	jne    400fe0 <main+0x130>
  400ffb:	48 8d 0c 2a          	lea    (%rdx,%rbp,1),%rcx
  400fff:	eb 58                	jmp    401059 <main+0x1a9>
  401001:	48 8d 44 24 38       	lea    0x38(%rsp),%rax
  401006:	45 31 ff             	xor    %r15d,%r15d
  401009:	b9 10 18 40 00       	mov    $0x401810,%ecx
  40100e:	4d 85 e4             	test   %r12,%r12
  401011:	0f 8e 4d 01 00 00    	jle    401164 <main+0x2b4>
  401017:	49 8d 56 01          	lea    0x1(%r14),%rdx
  40101b:	be 10 00 00 00       	mov    $0x10,%esi
  401020:	45 31 ff             	xor    %r15d,%r15d
  401023:	b9 10 18 40 00       	mov    $0x401810,%ecx
  401028:	0f 1f 84 00 00 00 00 	nopl   0x0(%rax,%rax,1)
  40102f:	00 
  401030:	0f 10 44 35 00       	movups 0x0(%rbp,%rsi,1),%xmm0
  401035:	0f 11 06             	movups %xmm0,(%rsi)
  401038:	0f 10 44 35 f0       	movups -0x10(%rbp,%rsi,1),%xmm0
  40103d:	0f 11 46 f0          	movups %xmm0,-0x10(%rsi)
  401041:	48 ff ca             	dec    %rdx
  401044:	48 83 c6 20          	add    $0x20,%rsi
  401048:	48 83 fa 01          	cmp    $0x1,%rdx
  40104c:	7f e2                	jg     401030 <main+0x180>
  40104e:	e9 11 01 00 00       	jmpq   401164 <main+0x2b4>
  401053:	4c 89 f8             	mov    %r15,%rax
  401056:	48 89 e9             	mov    %rbp,%rcx
  401059:	49 81 fc e0 00 00 00 	cmp    $0xe0,%r12
  401060:	0f 82 ce 00 00 00    	jb     401134 <main+0x284>
  401066:	66 2e 0f 1f 84 00 00 	nopw   %cs:0x0(%rax,%rax,1)
  40106d:	00 00 00 
  401070:	0f 10 01             	movups (%rcx),%xmm0
  401073:	0f 10 49 10          	movups 0x10(%rcx),%xmm1
  401077:	0f 11 48 10          	movups %xmm1,0x10(%rax)
  40107b:	0f 11 00             	movups %xmm0,(%rax)
  40107e:	0f 10 41 20          	movups 0x20(%rcx),%xmm0
  401082:	0f 10 49 30          	movups 0x30(%rcx),%xmm1
  401086:	0f 11 48 30          	movups %xmm1,0x30(%rax)
  40108a:	0f 11 40 20          	movups %xmm0,0x20(%rax)
  40108e:	0f 10 41 40          	movups 0x40(%rcx),%xmm0
  401092:	0f 10 49 50          	movups 0x50(%rcx),%xmm1
  401096:	0f 11 48 50          	movups %xmm1,0x50(%rax)
  40109a:	0f 11 40 40          	movups %xmm0,0x40(%rax)
  40109e:	0f 10 41 60          	movups 0x60(%rcx),%xmm0
  4010a2:	0f 10 49 70          	movups 0x70(%rcx),%xmm1
  4010a6:	0f 11 48 70          	movups %xmm1,0x70(%rax)
  4010aa:	0f 11 40 60          	movups %xmm0,0x60(%rax)
  4010ae:	0f 10 81 80 00 00 00 	movups 0x80(%rcx),%xmm0
  4010b5:	0f 10 89 90 00 00 00 	movups 0x90(%rcx),%xmm1
  4010bc:	0f 11 88 90 00 00 00 	movups %xmm1,0x90(%rax)
  4010c3:	0f 11 80 80 00 00 00 	movups %xmm0,0x80(%rax)
  4010ca:	0f 10 81 a0 00 00 00 	movups 0xa0(%rcx),%xmm0
  4010d1:	0f 10 89 b0 00 00 00 	movups 0xb0(%rcx),%xmm1
  4010d8:	0f 11 88 b0 00 00 00 	movups %xmm1,0xb0(%rax)
  4010df:	0f 11 80 a0 00 00 00 	movups %xmm0,0xa0(%rax)
  4010e6:	0f 10 81 c0 00 00 00 	movups 0xc0(%rcx),%xmm0
  4010ed:	0f 10 89 d0 00 00 00 	movups 0xd0(%rcx),%xmm1
  4010f4:	0f 11 88 d0 00 00 00 	movups %xmm1,0xd0(%rax)
  4010fb:	0f 11 80 c0 00 00 00 	movups %xmm0,0xc0(%rax)
  401102:	0f 10 81 e0 00 00 00 	movups 0xe0(%rcx),%xmm0
  401109:	0f 10 89 f0 00 00 00 	movups 0xf0(%rcx),%xmm1
  401110:	0f 11 88 f0 00 00 00 	movups %xmm1,0xf0(%rax)
  401117:	0f 11 80 e0 00 00 00 	movups %xmm0,0xe0(%rax)
  40111e:	48 81 c1 00 01 00 00 	add    $0x100,%rcx
  401125:	48 05 00 01 00 00    	add    $0x100,%rax
  40112b:	48 39 d9             	cmp    %rbx,%rcx
  40112e:	0f 85 3c ff ff ff    	jne    401070 <main+0x1c0>
  401134:	48 8b 7c 24 30       	mov    0x30(%rsp),%rdi
  401139:	48 85 ff             	test   %rdi,%rdi
  40113c:	74 05                	je     401143 <main+0x293>
  40113e:	e8 dd fa ff ff       	callq  400c20 <_ZdlPv@plt>
  401143:	4c 89 7c 24 30       	mov    %r15,0x30(%rsp)
  401148:	4c 89 f0             	mov    %r14,%rax
  40114b:	48 c1 e0 05          	shl    $0x5,%rax
  40114f:	4c 89 f9             	mov    %r15,%rcx
  401152:	48 01 c1             	add    %rax,%rcx
  401155:	48 89 4c 24 40       	mov    %rcx,0x40(%rsp)
  40115a:	48 8d 44 24 38       	lea    0x38(%rsp),%rax
  40115f:	48 8b 4c 24 28       	mov    0x28(%rsp),%rcx
  401164:	49 c1 e6 05          	shl    $0x5,%r14
  401168:	4d 01 fe             	add    %r15,%r14
  40116b:	4c 89 30             	mov    %r14,(%rax)
  40116e:	48 8b 41 08          	mov    0x8(%rcx),%rax
  401172:	48 8d 7c 24 28       	lea    0x28(%rsp),%rdi
  401177:	ff d0                	callq  *%rax
  401179:	48 89 2c 24          	mov    %rbp,(%rsp)
  40117d:	48 8b 44 24 48       	mov    0x48(%rsp),%rax
  401182:	48 3b 44 24 38       	cmp    0x38(%rsp),%rax
  401187:	0f 84 ea 01 00 00    	je     401377 <main+0x4c7>
  40118d:	4c 8d 74 24 18       	lea    0x18(%rsp),%r14
  401192:	4c 8d a4 24 d8 00 00 	lea    0xd8(%rsp),%r12
  401199:	00 
  40119a:	4c 8d bc 24 a8 00 00 	lea    0xa8(%rsp),%r15
  4011a1:	00 
  4011a2:	4c 8d 6c 24 08       	lea    0x8(%rsp),%r13
  4011a7:	66 0f 1f 84 00 00 00 	nopw   0x0(%rax,%rax,1)
  4011ae:	00 00 
  4011b0:	4c 8b 40 10          	mov    0x10(%rax),%r8
  4011b4:	48 8b 58 18          	mov    0x18(%rax),%rbx
  4011b8:	48 83 c0 20          	add    $0x20,%rax
  4011bc:	48 89 44 24 48       	mov    %rax,0x48(%rsp)
  4011c1:	4c 89 74 24 08       	mov    %r14,0x8(%rsp)
  4011c6:	48 c7 44 24 10 00 00 	movq   $0x0,0x10(%rsp)
  4011cd:	00 00 
  4011cf:	c6 44 24 18 00       	movb   $0x0,0x18(%rsp)
  4011d4:	31 c0                	xor    %eax,%eax
  4011d6:	4c 89 ff             	mov    %r15,%rdi
  4011d9:	e8 32 04 00 00       	callq  401610 <_ZN9__gnu_cxx12__to_xstringINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEcEET_PFiPT0_mPKS8_P13__va_list_tagEmSB_z>
  4011de:	48 8b 35 cb 12 20 00 	mov    0x2012cb(%rip),%rsi        # 6024b0 <_ZL9SEPARATOR>
  4011e5:	48 8b 15 cc 12 20 00 	mov    0x2012cc(%rip),%rdx        # 6024b8 <_ZL9SEPARATOR+0x8>
  4011ec:	4c 89 ff             	mov    %r15,%rdi
  4011ef:	e8 dc fa ff ff       	callq  400cd0 <_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcm@plt>
  4011f4:	4c 89 a4 24 c8 00 00 	mov    %r12,0xc8(%rsp)
  4011fb:	00 
  4011fc:	48 8b 10             	mov    (%rax),%rdx
  4011ff:	48 89 c1             	mov    %rax,%rcx
  401202:	48 83 c1 10          	add    $0x10,%rcx
  401206:	48 39 ca             	cmp    %rcx,%rdx
  401209:	74 25                	je     401230 <main+0x380>
  40120b:	48 89 94 24 c8 00 00 	mov    %rdx,0xc8(%rsp)
  401212:	00 
  401213:	48 8b 50 10          	mov    0x10(%rax),%rdx
  401217:	48 89 94 24 d8 00 00 	mov    %rdx,0xd8(%rsp)
  40121e:	00 
  40121f:	eb 17                	jmp    401238 <main+0x388>
  401221:	66 66 66 66 66 66 2e 	data16 data16 data16 data16 data16 nopw %cs:0x0(%rax,%rax,1)
  401228:	0f 1f 84 00 00 00 00 
  40122f:	00 
  401230:	0f 10 02             	movups (%rdx),%xmm0
  401233:	41 0f 11 04 24       	movups %xmm0,(%r12)
  401238:	48 8b 50 08          	mov    0x8(%rax),%rdx
  40123c:	48 89 94 24 d0 00 00 	mov    %rdx,0xd0(%rsp)
  401243:	00 
  401244:	48 89 08             	mov    %rcx,(%rax)
  401247:	48 c7 40 08 00 00 00 	movq   $0x0,0x8(%rax)
  40124e:	00 
  40124f:	c6 40 10 00          	movb   $0x0,0x10(%rax)
  401253:	48 8b b4 24 c8 00 00 	mov    0xc8(%rsp),%rsi
  40125a:	00 
  40125b:	48 8b 94 24 d0 00 00 	mov    0xd0(%rsp),%rdx
  401262:	00 
  401263:	4c 89 ef             	mov    %r13,%rdi
  401266:	e8 65 fa ff ff       	callq  400cd0 <_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcm@plt>
  40126b:	48 8b bc 24 c8 00 00 	mov    0xc8(%rsp),%rdi
  401272:	00 
  401273:	4c 39 e7             	cmp    %r12,%rdi
  401276:	74 05                	je     40127d <main+0x3cd>
  401278:	e8 a3 f9 ff ff       	callq  400c20 <_ZdlPv@plt>
  40127d:	48 8b bc 24 a8 00 00 	mov    0xa8(%rsp),%rdi
  401284:	00 
  401285:	48 8d 84 24 b8 00 00 	lea    0xb8(%rsp),%rax
  40128c:	00 
  40128d:	48 39 c7             	cmp    %rax,%rdi
  401290:	74 05                	je     401297 <main+0x3e7>
  401292:	e8 89 f9 ff ff       	callq  400c20 <_ZdlPv@plt>
  401297:	31 c0                	xor    %eax,%eax
  401299:	48 8d bc 24 e8 00 00 	lea    0xe8(%rsp),%rdi
  4012a0:	00 
  4012a1:	49 89 d8             	mov    %rbx,%r8
  4012a4:	e8 67 03 00 00       	callq  401610 <_ZN9__gnu_cxx12__to_xstringINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEcEET_PFiPT0_mPKS8_P13__va_list_tagEmSB_z>
  4012a9:	48 8b b4 24 e8 00 00 	mov    0xe8(%rsp),%rsi
  4012b0:	00 
  4012b1:	48 8b 94 24 f0 00 00 	mov    0xf0(%rsp),%rdx
  4012b8:	00 
  4012b9:	4c 89 ef             	mov    %r13,%rdi
  4012bc:	e8 0f fa ff ff       	callq  400cd0 <_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcm@plt>
  4012c1:	48 8b bc 24 e8 00 00 	mov    0xe8(%rsp),%rdi
  4012c8:	00 
  4012c9:	48 8d 84 24 f8 00 00 	lea    0xf8(%rsp),%rax
  4012d0:	00 
  4012d1:	48 39 c7             	cmp    %rax,%rdi
  4012d4:	74 05                	je     4012db <main+0x42b>
  4012d6:	e8 45 f9 ff ff       	callq  400c20 <_ZdlPv@plt>
  4012db:	48 8b 74 24 08       	mov    0x8(%rsp),%rsi
  4012e0:	48 8b 54 24 10       	mov    0x10(%rsp),%rdx
  4012e5:	bf 40 23 60 00       	mov    $0x602340,%edi
  4012ea:	e8 b1 f9 ff ff       	callq  400ca0 <_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@plt>
  4012ef:	48 89 c3             	mov    %rax,%rbx
  4012f2:	48 8b 03             	mov    (%rbx),%rax
  4012f5:	48 8b 40 e8          	mov    -0x18(%rax),%rax
  4012f9:	48 8b ac 03 f0 00 00 	mov    0xf0(%rbx,%rax,1),%rbp
  401300:	00 
  401301:	48 85 ed             	test   %rbp,%rbp
  401304:	74 68                	je     40136e <main+0x4be>
  401306:	80 7d 38 00          	cmpb   $0x0,0x38(%rbp)
  40130a:	74 14                	je     401320 <main+0x470>
  40130c:	8a 45 43             	mov    0x43(%rbp),%al
  40130f:	eb 29                	jmp    40133a <main+0x48a>
  401311:	66 66 66 66 66 66 2e 	data16 data16 data16 data16 data16 nopw %cs:0x0(%rax,%rax,1)
  401318:	0f 1f 84 00 00 00 00 
  40131f:	00 
  401320:	48 89 ef             	mov    %rbp,%rdi
  401323:	e8 08 f9 ff ff       	callq  400c30 <_ZNKSt5ctypeIcE13_M_widen_initEv@plt>
  401328:	48 8b 45 00          	mov    0x0(%rbp),%rax
  40132c:	48 8b 40 30          	mov    0x30(%rax),%rax
  401330:	be 0a 00 00 00       	mov    $0xa,%esi
  401335:	48 89 ef             	mov    %rbp,%rdi
  401338:	ff d0                	callq  *%rax
  40133a:	0f be f0             	movsbl %al,%esi
  40133d:	48 89 df             	mov    %rbx,%rdi
  401340:	e8 db f9 ff ff       	callq  400d20 <_ZNSo3putEc@plt>
  401345:	48 89 c7             	mov    %rax,%rdi
  401348:	e8 73 f9 ff ff       	callq  400cc0 <_ZNSo5flushEv@plt>
  40134d:	48 8b 7c 24 08       	mov    0x8(%rsp),%rdi
  401352:	4c 39 f7             	cmp    %r14,%rdi
  401355:	74 05                	je     40135c <main+0x4ac>
  401357:	e8 c4 f8 ff ff       	callq  400c20 <_ZdlPv@plt>
  40135c:	48 8b 44 24 48       	mov    0x48(%rsp),%rax
  401361:	48 3b 44 24 38       	cmp    0x38(%rsp),%rax
  401366:	0f 85 44 fe ff ff    	jne    4011b0 <main+0x300>
  40136c:	eb 09                	jmp    401377 <main+0x4c7>
  40136e:	e8 9d f8 ff ff       	callq  400c10 <_ZSt16__throw_bad_castv@plt>
  401373:	eb 91                	jmp    401306 <main+0x456>
  401375:	eb 3c                	jmp    4013b3 <main+0x503>
  401377:	48 c7 44 24 28 10 18 	movq   $0x401810,0x28(%rsp)
  40137e:	40 00 
  401380:	48 8b 7c 24 30       	mov    0x30(%rsp),%rdi
  401385:	48 85 ff             	test   %rdi,%rdi
  401388:	74 05                	je     40138f <main+0x4df>
  40138a:	e8 91 f8 ff ff       	callq  400c20 <_ZdlPv@plt>
  40138f:	48 8b 3c 24          	mov    (%rsp),%rdi
  401393:	48 85 ff             	test   %rdi,%rdi
  401396:	74 05                	je     40139d <main+0x4ed>
  401398:	e8 83 f8 ff ff       	callq  400c20 <_ZdlPv@plt>
  40139d:	31 c0                	xor    %eax,%eax
  40139f:	48 81 c4 08 01 00 00 	add    $0x108,%rsp
  4013a6:	5b                   	pop    %rbx
  4013a7:	41 5c                	pop    %r12
  4013a9:	41 5d                	pop    %r13
  4013ab:	41 5e                	pop    %r14
  4013ad:	41 5f                	pop    %r15
  4013af:	5d                   	pop    %rbp
  4013b0:	c3                   	retq   
  4013b1:	eb 00                	jmp    4013b3 <main+0x503>
  4013b3:	48 89 c3             	mov    %rax,%rbx
  4013b6:	48 8b 7c 24 08       	mov    0x8(%rsp),%rdi
  4013bb:	4c 39 f7             	cmp    %r14,%rdi
  4013be:	74 05                	je     4013c5 <main+0x515>
  4013c0:	e8 5b f8 ff ff       	callq  400c20 <_ZdlPv@plt>
  4013c5:	48 8b 2c 24          	mov    (%rsp),%rbp
  4013c9:	48 c7 44 24 28 10 18 	movq   $0x401810,0x28(%rsp)
  4013d0:	40 00 
  4013d2:	48 8b 7c 24 30       	mov    0x30(%rsp),%rdi
  4013d7:	48 85 ff             	test   %rdi,%rdi
  4013da:	74 05                	je     4013e1 <main+0x531>
  4013dc:	e8 3f f8 ff ff       	callq  400c20 <_ZdlPv@plt>
  4013e1:	48 85 ed             	test   %rbp,%rbp
  4013e4:	74 08                	je     4013ee <main+0x53e>
  4013e6:	48 89 ef             	mov    %rbp,%rdi
  4013e9:	e8 32 f8 ff ff       	callq  400c20 <_ZdlPv@plt>
  4013ee:	48 89 df             	mov    %rbx,%rdi
  4013f1:	e8 1a f9 ff ff       	callq  400d10 <_Unwind_Resume@plt>
  4013f6:	48 89 c3             	mov    %rax,%rbx
  4013f9:	eb 15                	jmp    401410 <main+0x560>
  4013fb:	48 89 c3             	mov    %rax,%rbx
  4013fe:	48 8b bc 24 c8 00 00 	mov    0xc8(%rsp),%rdi
  401405:	00 
  401406:	4c 39 e7             	cmp    %r12,%rdi
  401409:	74 05                	je     401410 <main+0x560>
  40140b:	e8 10 f8 ff ff       	callq  400c20 <_ZdlPv@plt>
  401410:	48 8b bc 24 a8 00 00 	mov    0xa8(%rsp),%rdi
  401417:	00 
  401418:	48 8d 84 24 b8 00 00 	lea    0xb8(%rsp),%rax
  40141f:	00 
  401420:	48 39 c7             	cmp    %rax,%rdi
  401423:	74 91                	je     4013b6 <main+0x506>
  401425:	e8 f6 f7 ff ff       	callq  400c20 <_ZdlPv@plt>
  40142a:	eb 8a                	jmp    4013b6 <main+0x506>
  40142c:	48 89 c3             	mov    %rax,%rbx
  40142f:	48 8b bc 24 e8 00 00 	mov    0xe8(%rsp),%rdi
  401436:	00 
  401437:	48 8d 84 24 f8 00 00 	lea    0xf8(%rsp),%rax
  40143e:	00 
  40143f:	48 39 c7             	cmp    %rax,%rdi
  401442:	0f 84 6e ff ff ff    	je     4013b6 <main+0x506>
  401448:	e8 d3 f7 ff ff       	callq  400c20 <_ZdlPv@plt>
  40144d:	e9 64 ff ff ff       	jmpq   4013b6 <main+0x506>
  401452:	48 89 c3             	mov    %rax,%rbx
  401455:	48 8b ac 24 90 00 00 	mov    0x90(%rsp),%rbp
  40145c:	00 
  40145d:	eb 82                	jmp    4013e1 <main+0x531>
  40145f:	e8 7c f8 ff ff       	callq  400ce0 <_ZSt17__throw_bad_allocv@plt>
  401464:	e9 2f fb ff ff       	jmpq   400f98 <main+0xe8>
  401469:	48 89 2c 24          	mov    %rbp,(%rsp)
  40146d:	48 89 c3             	mov    %rax,%rbx
  401470:	e9 50 ff ff ff       	jmpq   4013c5 <main+0x515>
  401475:	66 2e 0f 1f 84 00 00 	nopw   %cs:0x0(%rax,%rax,1)
  40147c:	00 00 00 
  40147f:	90                   	nop

0000000000401480 <_ZN24CollectionSourceOperatorISt5tupleIJllllEEE9printNameEv>:
  401480:	bf 40 23 60 00       	mov    $0x602340,%edi
  401485:	be 88 18 40 00       	mov    $0x401888,%esi
  40148a:	ba 0e 00 00 00       	mov    $0xe,%edx
  40148f:	e9 0c f8 ff ff       	jmpq   400ca0 <_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@plt>
  401494:	66 2e 0f 1f 84 00 00 	nopw   %cs:0x0(%rax,%rax,1)
  40149b:	00 00 00 
  40149e:	66 90                	xchg   %ax,%ax

00000000004014a0 <_ZN24CollectionSourceOperatorISt5tupleIJllllEEE4openEv>:
  4014a0:	48 8b 47 08          	mov    0x8(%rdi),%rax
  4014a4:	48 89 47 20          	mov    %rax,0x20(%rdi)
  4014a8:	c3                   	retq   
  4014a9:	0f 1f 80 00 00 00 00 	nopl   0x0(%rax)

00000000004014b0 <_ZNSt6vectorISt5tupleIJllllEESaIS1_EE19_M_emplace_back_auxIJS1_EEEvDpOT_>:
  4014b0:	41 57                	push   %r15
  4014b2:	41 56                	push   %r14
  4014b4:	41 55                	push   %r13
  4014b6:	41 54                	push   %r12
  4014b8:	53                   	push   %rbx
  4014b9:	49 89 f4             	mov    %rsi,%r12
  4014bc:	49 89 fe             	mov    %rdi,%r14
  4014bf:	49 8b 46 08          	mov    0x8(%r14),%rax
  4014c3:	49 2b 06             	sub    (%r14),%rax
  4014c6:	48 c1 f8 05          	sar    $0x5,%rax
  4014ca:	b9 01 00 00 00       	mov    $0x1,%ecx
  4014cf:	48 0f 45 c8          	cmovne %rax,%rcx
  4014d3:	4c 8d 3c 01          	lea    (%rcx,%rax,1),%r15
  4014d7:	4c 89 fa             	mov    %r15,%rdx
  4014da:	48 c1 ea 3b          	shr    $0x3b,%rdx
  4014de:	48 ba ff ff ff ff ff 	movabs $0x7ffffffffffffff,%rdx
  4014e5:	ff ff 07 
  4014e8:	4c 0f 45 fa          	cmovne %rdx,%r15
  4014ec:	48 01 c1             	add    %rax,%rcx
  4014ef:	4c 0f 42 fa          	cmovb  %rdx,%r15
  4014f3:	4c 89 f8             	mov    %r15,%rax
  4014f6:	48 c1 e8 3b          	shr    $0x3b,%rax
  4014fa:	0f 85 0a 01 00 00    	jne    40160a <_ZNSt6vectorISt5tupleIJllllEESaIS1_EE19_M_emplace_back_auxIJS1_EEEvDpOT_+0x15a>
  401500:	49 c1 e7 05          	shl    $0x5,%r15
  401504:	4c 89 ff             	mov    %r15,%rdi
  401507:	e8 f4 f7 ff ff       	callq  400d00 <_Znwm@plt>
  40150c:	49 89 c5             	mov    %rax,%r13
  40150f:	49 8b 3e             	mov    (%r14),%rdi
  401512:	49 8b 46 08          	mov    0x8(%r14),%rax
  401516:	48 89 c1             	mov    %rax,%rcx
  401519:	48 29 f9             	sub    %rdi,%rcx
  40151c:	41 0f 10 04 24       	movups (%r12),%xmm0
  401521:	41 0f 11 44 0d 00    	movups %xmm0,0x0(%r13,%rcx,1)
  401527:	41 0f 10 44 24 10    	movups 0x10(%r12),%xmm0
  40152d:	41 0f 11 44 0d 10    	movups %xmm0,0x10(%r13,%rcx,1)
  401533:	4c 89 eb             	mov    %r13,%rbx
  401536:	0f 84 a8 00 00 00    	je     4015e4 <_ZNSt6vectorISt5tupleIJllllEESaIS1_EE19_M_emplace_back_auxIJS1_EEEvDpOT_+0x134>
  40153c:	48 8d 48 e0          	lea    -0x20(%rax),%rcx
  401540:	48 29 f9             	sub    %rdi,%rcx
  401543:	49 89 c8             	mov    %rcx,%r8
  401546:	49 c1 e8 05          	shr    $0x5,%r8
  40154a:	48 0f ba e1 05       	bt     $0x5,%rcx
  40154f:	72 29                	jb     40157a <_ZNSt6vectorISt5tupleIJllllEESaIS1_EE19_M_emplace_back_auxIJS1_EEEvDpOT_+0xca>
  401551:	48 8b 17             	mov    (%rdi),%rdx
  401554:	49 89 55 00          	mov    %rdx,0x0(%r13)
  401558:	48 8b 57 08          	mov    0x8(%rdi),%rdx
  40155c:	49 89 55 08          	mov    %rdx,0x8(%r13)
  401560:	48 8b 57 10          	mov    0x10(%rdi),%rdx
  401564:	49 89 55 10          	mov    %rdx,0x10(%r13)
  401568:	48 8b 57 18          	mov    0x18(%rdi),%rdx
  40156c:	49 89 55 18          	mov    %rdx,0x18(%r13)
  401570:	48 8d 77 20          	lea    0x20(%rdi),%rsi
  401574:	49 8d 55 20          	lea    0x20(%r13),%rdx
  401578:	eb 06                	jmp    401580 <_ZNSt6vectorISt5tupleIJllllEESaIS1_EE19_M_emplace_back_auxIJS1_EEEvDpOT_+0xd0>
  40157a:	4c 89 ea             	mov    %r13,%rdx
  40157d:	48 89 fe             	mov    %rdi,%rsi
  401580:	4d 85 c0             	test   %r8,%r8
  401583:	74 56                	je     4015db <_ZNSt6vectorISt5tupleIJllllEESaIS1_EE19_M_emplace_back_auxIJS1_EEEvDpOT_+0x12b>
  401585:	66 66 2e 0f 1f 84 00 	data16 nopw %cs:0x0(%rax,%rax,1)
  40158c:	00 00 00 00 
  401590:	48 8b 1e             	mov    (%rsi),%rbx
  401593:	48 89 1a             	mov    %rbx,(%rdx)
  401596:	48 8b 5e 08          	mov    0x8(%rsi),%rbx
  40159a:	48 89 5a 08          	mov    %rbx,0x8(%rdx)
  40159e:	48 8b 5e 10          	mov    0x10(%rsi),%rbx
  4015a2:	48 89 5a 10          	mov    %rbx,0x10(%rdx)
  4015a6:	48 8b 5e 18          	mov    0x18(%rsi),%rbx
  4015aa:	48 89 5a 18          	mov    %rbx,0x18(%rdx)
  4015ae:	48 8b 5e 20          	mov    0x20(%rsi),%rbx
  4015b2:	48 89 5a 20          	mov    %rbx,0x20(%rdx)
  4015b6:	48 8b 5e 28          	mov    0x28(%rsi),%rbx
  4015ba:	48 89 5a 28          	mov    %rbx,0x28(%rdx)
  4015be:	48 8b 5e 30          	mov    0x30(%rsi),%rbx
  4015c2:	48 89 5a 30          	mov    %rbx,0x30(%rdx)
  4015c6:	48 8b 5e 38          	mov    0x38(%rsi),%rbx
  4015ca:	48 89 5a 38          	mov    %rbx,0x38(%rdx)
  4015ce:	48 83 c6 40          	add    $0x40,%rsi
  4015d2:	48 83 c2 40          	add    $0x40,%rdx
  4015d6:	48 39 c6             	cmp    %rax,%rsi
  4015d9:	75 b5                	jne    401590 <_ZNSt6vectorISt5tupleIJllllEESaIS1_EE19_M_emplace_back_auxIJS1_EEEvDpOT_+0xe0>
  4015db:	48 83 e1 e0          	and    $0xffffffffffffffe0,%rcx
  4015df:	4a 8d 5c 29 20       	lea    0x20(%rcx,%r13,1),%rbx
  4015e4:	48 83 c3 20          	add    $0x20,%rbx
  4015e8:	48 85 ff             	test   %rdi,%rdi
  4015eb:	74 05                	je     4015f2 <_ZNSt6vectorISt5tupleIJllllEESaIS1_EE19_M_emplace_back_auxIJS1_EEEvDpOT_+0x142>
  4015ed:	e8 2e f6 ff ff       	callq  400c20 <_ZdlPv@plt>
  4015f2:	4d 89 2e             	mov    %r13,(%r14)
  4015f5:	49 89 5e 08          	mov    %rbx,0x8(%r14)
  4015f9:	4d 01 fd             	add    %r15,%r13
  4015fc:	4d 89 6e 10          	mov    %r13,0x10(%r14)
  401600:	5b                   	pop    %rbx
  401601:	41 5c                	pop    %r12
  401603:	41 5d                	pop    %r13
  401605:	41 5e                	pop    %r14
  401607:	41 5f                	pop    %r15
  401609:	c3                   	retq   
  40160a:	e8 d1 f6 ff ff       	callq  400ce0 <_ZSt17__throw_bad_allocv@plt>
  40160f:	90                   	nop

0000000000401610 <_ZN9__gnu_cxx12__to_xstringINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEcEET_PFiPT0_mPKS8_P13__va_list_tagEmSB_z>:
  401610:	41 56                	push   %r14
  401612:	53                   	push   %rbx
  401613:	48 81 ec f8 00 00 00 	sub    $0xf8,%rsp
  40161a:	49 89 fe             	mov    %rdi,%r14
  40161d:	84 c0                	test   %al,%al
  40161f:	74 31                	je     401652 <_ZN9__gnu_cxx12__to_xstringINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEcEET_PFiPT0_mPKS8_P13__va_list_tagEmSB_z+0x42>
  401621:	0f 29 44 24 30       	movaps %xmm0,0x30(%rsp)
  401626:	0f 29 4c 24 40       	movaps %xmm1,0x40(%rsp)
  40162b:	0f 29 54 24 50       	movaps %xmm2,0x50(%rsp)
  401630:	0f 29 5c 24 60       	movaps %xmm3,0x60(%rsp)
  401635:	0f 29 64 24 70       	movaps %xmm4,0x70(%rsp)
  40163a:	0f 29 ac 24 80 00 00 	movaps %xmm5,0x80(%rsp)
  401641:	00 
  401642:	0f 29 b4 24 90 00 00 	movaps %xmm6,0x90(%rsp)
  401649:	00 
  40164a:	0f 29 bc 24 a0 00 00 	movaps %xmm7,0xa0(%rsp)
  401651:	00 
  401652:	4c 89 4c 24 28       	mov    %r9,0x28(%rsp)
  401657:	4c 89 44 24 20       	mov    %r8,0x20(%rsp)
  40165c:	48 8d 04 24          	lea    (%rsp),%rax
  401660:	48 89 84 24 e0 00 00 	mov    %rax,0xe0(%rsp)
  401667:	00 
  401668:	48 8d 84 24 10 01 00 	lea    0x110(%rsp),%rax
  40166f:	00 
  401670:	48 89 84 24 d8 00 00 	mov    %rax,0xd8(%rsp)
  401677:	00 
  401678:	c7 84 24 d4 00 00 00 	movl   $0x30,0xd4(%rsp)
  40167f:	30 00 00 00 
  401683:	c7 84 24 d0 00 00 00 	movl   $0x20,0xd0(%rsp)
  40168a:	20 00 00 00 
  40168e:	48 8d bc 24 b0 00 00 	lea    0xb0(%rsp),%rdi
  401695:	00 
  401696:	48 8d 8c 24 d0 00 00 	lea    0xd0(%rsp),%rcx
  40169d:	00 
  40169e:	be 20 00 00 00       	mov    $0x20,%esi
  4016a3:	ba 97 18 40 00       	mov    $0x401897,%edx
  4016a8:	e8 03 f6 ff ff       	callq  400cb0 <vsnprintf@plt>
  4016ad:	48 63 d8             	movslq %eax,%rbx
  4016b0:	49 8d 46 10          	lea    0x10(%r14),%rax
  4016b4:	49 89 06             	mov    %rax,(%r14)
  4016b7:	48 89 9c 24 f0 00 00 	mov    %rbx,0xf0(%rsp)
  4016be:	00 
  4016bf:	83 fb 0f             	cmp    $0xf,%ebx
  4016c2:	76 21                	jbe    4016e5 <_ZN9__gnu_cxx12__to_xstringINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEcEET_PFiPT0_mPKS8_P13__va_list_tagEmSB_z+0xd5>
  4016c4:	48 8d b4 24 f0 00 00 	lea    0xf0(%rsp),%rsi
  4016cb:	00 
  4016cc:	31 d2                	xor    %edx,%edx
  4016ce:	4c 89 f7             	mov    %r14,%rdi
  4016d1:	e8 6a f5 ff ff       	callq  400c40 <_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERmm@plt>
  4016d6:	49 89 06             	mov    %rax,(%r14)
  4016d9:	48 8b 8c 24 f0 00 00 	mov    0xf0(%rsp),%rcx
  4016e0:	00 
  4016e1:	49 89 4e 10          	mov    %rcx,0x10(%r14)
  4016e5:	48 85 db             	test   %rbx,%rbx
  4016e8:	74 24                	je     40170e <_ZN9__gnu_cxx12__to_xstringINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEcEET_PFiPT0_mPKS8_P13__va_list_tagEmSB_z+0xfe>
  4016ea:	48 83 fb 01          	cmp    $0x1,%rbx
  4016ee:	75 0b                	jne    4016fb <_ZN9__gnu_cxx12__to_xstringINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEcEET_PFiPT0_mPKS8_P13__va_list_tagEmSB_z+0xeb>
  4016f0:	8a 8c 24 b0 00 00 00 	mov    0xb0(%rsp),%cl
  4016f7:	88 08                	mov    %cl,(%rax)
  4016f9:	eb 13                	jmp    40170e <_ZN9__gnu_cxx12__to_xstringINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEcEET_PFiPT0_mPKS8_P13__va_list_tagEmSB_z+0xfe>
  4016fb:	48 8d b4 24 b0 00 00 	lea    0xb0(%rsp),%rsi
  401702:	00 
  401703:	48 89 c7             	mov    %rax,%rdi
  401706:	48 89 da             	mov    %rbx,%rdx
  401709:	e8 22 f6 ff ff       	callq  400d30 <memcpy@plt>
  40170e:	48 8b 84 24 f0 00 00 	mov    0xf0(%rsp),%rax
  401715:	00 
  401716:	49 89 46 08          	mov    %rax,0x8(%r14)
  40171a:	49 8b 0e             	mov    (%r14),%rcx
  40171d:	c6 04 01 00          	movb   $0x0,(%rcx,%rax,1)
  401721:	4c 89 f0             	mov    %r14,%rax
  401724:	48 81 c4 f8 00 00 00 	add    $0xf8,%rsp
  40172b:	5b                   	pop    %rbx
  40172c:	41 5e                	pop    %r14
  40172e:	c3                   	retq   
  40172f:	90                   	nop

0000000000401730 <__libc_csu_init>:
  401730:	41 57                	push   %r15
  401732:	41 56                	push   %r14
  401734:	41 89 ff             	mov    %edi,%r15d
  401737:	41 55                	push   %r13
  401739:	41 54                	push   %r12
  40173b:	4c 8d 25 be 08 20 00 	lea    0x2008be(%rip),%r12        # 602000 <__frame_dummy_init_array_entry>
  401742:	55                   	push   %rbp
  401743:	48 8d 2d c6 08 20 00 	lea    0x2008c6(%rip),%rbp        # 602010 <__init_array_end>
  40174a:	53                   	push   %rbx
  40174b:	49 89 f6             	mov    %rsi,%r14
  40174e:	49 89 d5             	mov    %rdx,%r13
  401751:	4c 29 e5             	sub    %r12,%rbp
  401754:	48 83 ec 08          	sub    $0x8,%rsp
  401758:	48 c1 fd 03          	sar    $0x3,%rbp
  40175c:	e8 77 f4 ff ff       	callq  400bd8 <_init>
  401761:	48 85 ed             	test   %rbp,%rbp
  401764:	74 20                	je     401786 <__libc_csu_init+0x56>
  401766:	31 db                	xor    %ebx,%ebx
  401768:	0f 1f 84 00 00 00 00 	nopl   0x0(%rax,%rax,1)
  40176f:	00 
  401770:	4c 89 ea             	mov    %r13,%rdx
  401773:	4c 89 f6             	mov    %r14,%rsi
  401776:	44 89 ff             	mov    %r15d,%edi
  401779:	41 ff 14 dc          	callq  *(%r12,%rbx,8)
  40177d:	48 83 c3 01          	add    $0x1,%rbx
  401781:	48 39 eb             	cmp    %rbp,%rbx
  401784:	75 ea                	jne    401770 <__libc_csu_init+0x40>
  401786:	48 83 c4 08          	add    $0x8,%rsp
  40178a:	5b                   	pop    %rbx
  40178b:	5d                   	pop    %rbp
  40178c:	41 5c                	pop    %r12
  40178e:	41 5d                	pop    %r13
  401790:	41 5e                	pop    %r14
  401792:	41 5f                	pop    %r15
  401794:	c3                   	retq   
  401795:	90                   	nop
  401796:	66 2e 0f 1f 84 00 00 	nopw   %cs:0x0(%rax,%rax,1)
  40179d:	00 00 00 

00000000004017a0 <__libc_csu_fini>:
  4017a0:	f3 c3                	repz retq 

Disassembly of section .fini:

00000000004017a4 <_fini>:
  4017a4:	48 83 ec 08          	sub    $0x8,%rsp
  4017a8:	48 83 c4 08          	add    $0x8,%rsp
  4017ac:	c3                   	retq   

Disassembly of section .rodata:

00000000004017b0 <_IO_stdin_used>:
  4017b0:	01 00                	add    %eax,(%rax)
  4017b2:	02 00                	add    (%rax),%al
	...
  4017c0:	04 00                	add    $0x0,%al
  4017c2:	00 00                	add    %al,(%rax)
  4017c4:	00 00                	add    %al,(%rax)
  4017c6:	00 00                	add    %al,(%rax)
  4017c8:	03 00                	add    (%rax),%eax
  4017ca:	00 00                	add    %al,(%rax)
  4017cc:	00 00                	add    %al,(%rax)
  4017ce:	00 00                	add    %al,(%rax)
  4017d0:	02 00                	add    (%rax),%al
  4017d2:	00 00                	add    %al,(%rax)
  4017d4:	00 00                	add    %al,(%rax)
  4017d6:	00 00                	add    %al,(%rax)
  4017d8:	01 00                	add    %eax,(%rax)
  4017da:	00 00                	add    %al,(%rax)
  4017dc:	00 00                	add    %al,(%rax)
  4017de:	00 00                	add    %al,(%rax)
  4017e0:	08 00                	or     %al,(%rax)
  4017e2:	00 00                	add    %al,(%rax)
  4017e4:	00 00                	add    %al,(%rax)
  4017e6:	00 00                	add    %al,(%rax)
  4017e8:	07                   	(bad)  
  4017e9:	00 00                	add    %al,(%rax)
  4017eb:	00 00                	add    %al,(%rax)
  4017ed:	00 00                	add    %al,(%rax)
  4017ef:	00 06                	add    %al,(%rsi)
  4017f1:	00 00                	add    %al,(%rax)
  4017f3:	00 00                	add    %al,(%rax)
  4017f5:	00 00                	add    %al,(%rax)
  4017f7:	00 05 00 00 00 00    	add    %al,0x0(%rip)        # 4017fd <_IO_stdin_used+0x4d>
  4017fd:	00 00                	add    %al,(%rax)
	...

0000000000401800 <_ZTV24CollectionSourceOperatorISt5tupleIJllllEEE>:
	...
  401808:	70 18                	jo     401822 <_ZTS24CollectionSourceOperatorISt5tupleIJllllEEE+0x2>
  40180a:	40 00 00             	add    %al,(%rax)
  40180d:	00 00                	add    %al,(%rax)
  40180f:	00 80 14 40 00 00    	add    %al,0x4014(%rax)
  401815:	00 00                	add    %al,(%rax)
  401817:	00 a0 14 40 00 00    	add    %ah,0x4014(%rax)
  40181d:	00 00                	add    %al,(%rax)
	...

0000000000401820 <_ZTS24CollectionSourceOperatorISt5tupleIJllllEEE>:
  401820:	32 34 43             	xor    (%rbx,%rax,2),%dh
  401823:	6f                   	outsl  %ds:(%rsi),(%dx)
  401824:	6c                   	insb   (%dx),%es:(%rdi)
  401825:	6c                   	insb   (%dx),%es:(%rdi)
  401826:	65 63 74 69 6f       	movslq %gs:0x6f(%rcx,%rbp,2),%esi
  40182b:	6e                   	outsb  %ds:(%rsi),(%dx)
  40182c:	53                   	push   %rbx
  40182d:	6f                   	outsl  %ds:(%rsi),(%dx)
  40182e:	75 72                	jne    4018a2 <__GNU_EH_FRAME_HDR+0x6>
  401830:	63 65 4f             	movslq 0x4f(%rbp),%esp
  401833:	70 65                	jo     40189a <_ZTI24CollectionSourceOperatorISt5tupleIJllllEEE+0x2a>
  401835:	72 61                	jb     401898 <_ZTI24CollectionSourceOperatorISt5tupleIJllllEEE+0x28>
  401837:	74 6f                	je     4018a8 <__GNU_EH_FRAME_HDR+0xc>
  401839:	72 49                	jb     401884 <_ZTI24CollectionSourceOperatorISt5tupleIJllllEEE+0x14>
  40183b:	53                   	push   %rbx
  40183c:	74 35                	je     401873 <_ZTI24CollectionSourceOperatorISt5tupleIJllllEEE+0x3>
  40183e:	74 75                	je     4018b5 <__GNU_EH_FRAME_HDR+0x19>
  401840:	70 6c                	jo     4018ae <__GNU_EH_FRAME_HDR+0x12>
  401842:	65 49                	gs rex.WB
  401844:	4a 6c                	rex.WX insb (%dx),%es:(%rdi)
  401846:	6c                   	insb   (%dx),%es:(%rdi)
  401847:	6c                   	insb   (%dx),%es:(%rdi)
  401848:	6c                   	insb   (%dx),%es:(%rdi)
  401849:	45                   	rex.RB
  40184a:	45                   	rex.RB
  40184b:	45 00 38             	add    %r15b,(%r8)

000000000040184d <_ZTS8Operator>:
  40184d:	38 4f 70             	cmp    %cl,0x70(%rdi)
  401850:	65 72 61             	gs jb  4018b4 <__GNU_EH_FRAME_HDR+0x18>
  401853:	74 6f                	je     4018c4 <__GNU_EH_FRAME_HDR+0x28>
  401855:	72 00                	jb     401857 <_ZTS8Operator+0xa>
	...

0000000000401858 <_ZTI8Operator>:
  401858:	f0 22 60 00          	lock and 0x0(%rax),%ah
  40185c:	00 00                	add    %al,(%rax)
  40185e:	00 00                	add    %al,(%rax)
  401860:	4d 18 40 00          	rex.WRB sbb %r8b,0x0(%r8)
	...

0000000000401870 <_ZTI24CollectionSourceOperatorISt5tupleIJllllEEE>:
  401870:	60                   	(bad)  
  401871:	24 60                	and    $0x60,%al
  401873:	00 00                	add    %al,(%rax)
  401875:	00 00                	add    %al,(%rax)
  401877:	00 20                	add    %ah,(%rax)
  401879:	18 40 00             	sbb    %al,0x0(%rax)
  40187c:	00 00                	add    %al,(%rax)
  40187e:	00 00                	add    %al,(%rax)
  401880:	58                   	pop    %rax
  401881:	18 40 00             	sbb    %al,0x0(%rax)
  401884:	00 00                	add    %al,(%rax)
  401886:	00 00                	add    %al,(%rax)
  401888:	63 6f 6c             	movslq 0x6c(%rdi),%ebp
  40188b:	6c                   	insb   (%dx),%es:(%rdi)
  40188c:	65 63 74 69 6f       	movslq %gs:0x6f(%rcx,%rbp,2),%esi
  401891:	6e                   	outsb  %ds:(%rsi),(%dx)
  401892:	20 6f 70             	and    %ch,0x70(%rdi)
  401895:	0a 00                	or     (%rax),%al
  401897:	25                   	.byte 0x25
  401898:	6c                   	insb   (%dx),%es:(%rdi)
  401899:	64                   	fs
	...

Disassembly of section .eh_frame_hdr:

000000000040189c <__GNU_EH_FRAME_HDR>:
  40189c:	01 1b                	add    %ebx,(%rbx)
  40189e:	03 3b                	add    (%rbx),%edi
  4018a0:	58                   	pop    %rax
  4018a1:	00 00                	add    %al,(%rax)
  4018a3:	00 0a                	add    %cl,(%rdx)
  4018a5:	00 00                	add    %al,(%rax)
  4018a7:	00 64 f3 ff          	add    %ah,-0x1(%rbx,%rsi,8)
  4018ab:	ff a4 00 00 00 b4 f4 	jmpq   *-0xb4c0000(%rax,%rax,1)
  4018b2:	ff                   	(bad)  
  4018b3:	ff a4 01 00 00 14 f5 	jmpq   *-0xaec0000(%rcx,%rax,1)
  4018ba:	ff                   	(bad)  
  4018bb:	ff 74 00 00          	pushq  0x0(%rax,%rax,1)
  4018bf:	00 14 f6             	add    %dl,(%rsi,%rsi,8)
  4018c2:	ff                   	(bad)  
  4018c3:	ff                   	(bad)  
  4018c4:	ec                   	in     (%dx),%al
  4018c5:	00 00                	add    %al,(%rax)
  4018c7:	00 e4                	add    %ah,%ah
  4018c9:	fb                   	sti    
  4018ca:	ff                   	(bad)  
  4018cb:	ff 24 01             	jmpq   *(%rcx,%rax,1)
  4018ce:	00 00                	add    %al,(%rax)
  4018d0:	04 fc                	add    $0xfc,%al
  4018d2:	ff                   	(bad)  
  4018d3:	ff                   	(bad)  
  4018d4:	3c 01                	cmp    $0x1,%al
  4018d6:	00 00                	add    %al,(%rax)
  4018d8:	14 fc                	adc    $0xfc,%al
  4018da:	ff                   	(bad)  
  4018db:	ff 54 01 00          	callq  *0x0(%rcx,%rax,1)
  4018df:	00 74 fd ff          	add    %dh,-0x1(%rbp,%rdi,8)
  4018e3:	ff 84 01 00 00 94 fe 	incl   -0x16c0000(%rcx,%rax,1)
  4018ea:	ff                   	(bad)  
  4018eb:	ff                   	(bad)  
  4018ec:	bc 01 00 00 04       	mov    $0x4000001,%esp
  4018f1:	ff                   	(bad)  
  4018f2:	ff                   	(bad)  
  4018f3:	ff 04 02             	incl   (%rdx,%rax,1)
	...

Disassembly of section .eh_frame:

00000000004018f8 <__FRAME_END__-0x1c0>:
  4018f8:	14 00                	adc    $0x0,%al
  4018fa:	00 00                	add    %al,(%rax)
  4018fc:	00 00                	add    %al,(%rax)
  4018fe:	00 00                	add    %al,(%rax)
  401900:	01 7a 52             	add    %edi,0x52(%rdx)
  401903:	00 01                	add    %al,(%rcx)
  401905:	78 10                	js     401917 <__GNU_EH_FRAME_HDR+0x7b>
  401907:	01 1b                	add    %ebx,(%rbx)
  401909:	0c 07                	or     $0x7,%al
  40190b:	08 90 01 07 10 14    	or     %dl,0x14100701(%rax)
  401911:	00 00                	add    %al,(%rax)
  401913:	00 1c 00             	add    %bl,(%rax,%rax,1)
  401916:	00 00                	add    %al,(%rax)
  401918:	98                   	cwtl   
  401919:	f4                   	hlt    
  40191a:	ff                   	(bad)  
  40191b:	ff 2a                	ljmp   *(%rdx)
	...
  401925:	00 00                	add    %al,(%rax)
  401927:	00 14 00             	add    %dl,(%rax,%rax,1)
  40192a:	00 00                	add    %al,(%rax)
  40192c:	00 00                	add    %al,(%rax)
  40192e:	00 00                	add    %al,(%rax)
  401930:	01 7a 52             	add    %edi,0x52(%rdx)
  401933:	00 01                	add    %al,(%rcx)
  401935:	78 10                	js     401947 <__GNU_EH_FRAME_HDR+0xab>
  401937:	01 1b                	add    %ebx,(%rbx)
  401939:	0c 07                	or     $0x7,%al
  40193b:	08 90 01 00 00 24    	or     %dl,0x24000001(%rax)
  401941:	00 00                	add    %al,(%rax)
  401943:	00 1c 00             	add    %bl,(%rax,%rax,1)
  401946:	00 00                	add    %al,(%rax)
  401948:	b8 f2 ff ff 40       	mov    $0x40fffff2,%eax
  40194d:	01 00                	add    %eax,(%rax)
  40194f:	00 00                	add    %al,(%rax)
  401951:	0e                   	(bad)  
  401952:	10 46 0e             	adc    %al,0xe(%rsi)
  401955:	18 4a 0f             	sbb    %cl,0xf(%rdx)
  401958:	0b 77 08             	or     0x8(%rdi),%esi
  40195b:	80 00 3f             	addb   $0x3f,(%rax)
  40195e:	1a 3b                	sbb    (%rbx),%bh
  401960:	2a 33                	sub    (%rbx),%dh
  401962:	24 22                	and    $0x22,%al
  401964:	00 00                	add    %al,(%rax)
  401966:	00 00                	add    %al,(%rax)
  401968:	1c 00                	sbb    $0x0,%al
  40196a:	00 00                	add    %al,(%rax)
  40196c:	00 00                	add    %al,(%rax)
  40196e:	00 00                	add    %al,(%rax)
  401970:	01 7a 50             	add    %edi,0x50(%rdx)
  401973:	4c 52                	rex.WR push %rdx
  401975:	00 01                	add    %al,(%rcx)
  401977:	78 10                	js     401989 <__GNU_EH_FRAME_HDR+0xed>
  401979:	07                   	(bad)  
  40197a:	03 f0                	add    %eax,%esi
  40197c:	0c 40                	or     $0x40,%al
  40197e:	00 03                	add    %al,(%rbx)
  401980:	1b 0c 07             	sbb    (%rdi,%rax,1),%ecx
  401983:	08 90 01 00 00 34    	or     %dl,0x34000001(%rax)
  401989:	00 00                	add    %al,(%rax)
  40198b:	00 24 00             	add    %ah,(%rax,%rax,1)
  40198e:	00 00                	add    %al,(%rax)
  401990:	20 f5                	and    %dh,%ch
  401992:	ff                   	(bad)  
  401993:	ff c5                	inc    %ebp
  401995:	05 00 00 04 bc       	add    $0xbc040000,%eax
  40199a:	1a 40 00             	sbb    0x0(%rax),%al
  40199d:	41 0e                	rex.B (bad) 
  40199f:	10 42 0e             	adc    %al,0xe(%rdx)
  4019a2:	18 42 0e             	sbb    %al,0xe(%rdx)
  4019a5:	20 42 0e             	and    %al,0xe(%rdx)
  4019a8:	28 42 0e             	sub    %al,0xe(%rdx)
  4019ab:	30 41 0e             	xor    %al,0xe(%rcx)
  4019ae:	38 47 0e             	cmp    %al,0xe(%rdi)
  4019b1:	c0 02 83             	rolb   $0x83,(%rdx)
  4019b4:	07                   	(bad)  
  4019b5:	8c 06                	mov    %es,(%rsi)
  4019b7:	8d 05 8e 04 8f 03    	lea    0x38f048e(%rip),%eax        # 3cf1e4b <_end+0x36ef973>
  4019bd:	86 02                	xchg   %al,(%rdx)
  4019bf:	00 14 00             	add    %dl,(%rax,%rax,1)
  4019c2:	00 00                	add    %al,(%rax)
  4019c4:	9c                   	pushfq 
  4019c5:	00 00                	add    %al,(%rax)
  4019c7:	00 b8 fa ff ff 14    	add    %bh,0x14fffffa(%rax)
	...
  4019d5:	00 00                	add    %al,(%rax)
  4019d7:	00 14 00             	add    %dl,(%rax,%rax,1)
  4019da:	00 00                	add    %al,(%rax)
  4019dc:	b4 00                	mov    $0x0,%ah
  4019de:	00 00                	add    %al,(%rax)
  4019e0:	c0 fa ff             	sar    $0xff,%dl
  4019e3:	ff 09                	decl   (%rcx)
	...
  4019ed:	00 00                	add    %al,(%rax)
  4019ef:	00 2c 00             	add    %ch,(%rax,%rax,1)
  4019f2:	00 00                	add    %al,(%rax)
  4019f4:	cc                   	int3   
  4019f5:	00 00                	add    %al,(%rax)
  4019f7:	00 b8 fa ff ff 5f    	add    %bh,0x5ffffffa(%rax)
  4019fd:	01 00                	add    %eax,(%rax)
  4019ff:	00 00                	add    %al,(%rax)
  401a01:	42 0e                	rex.X (bad) 
  401a03:	10 42 0e             	adc    %al,0xe(%rdx)
  401a06:	18 42 0e             	sbb    %al,0xe(%rdx)
  401a09:	20 42 0e             	and    %al,0xe(%rdx)
  401a0c:	28 41 0e             	sub    %al,0xe(%rcx)
  401a0f:	30 83 06 8c 05 8d    	xor    %al,-0x72fa73fa(%rbx)
  401a15:	04 8e                	add    $0x8e,%al
  401a17:	03 8f 02 00 00 00    	add    0x2(%rdi),%ecx
  401a1d:	00 00                	add    %al,(%rax)
  401a1f:	00 1c 00             	add    %bl,(%rax,%rax,1)
  401a22:	00 00                	add    %al,(%rax)
  401a24:	fc                   	cld    
  401a25:	00 00                	add    %al,(%rax)
  401a27:	00 e8                	add    %ch,%al
  401a29:	fb                   	sti    
  401a2a:	ff                   	(bad)  
  401a2b:	ff 1f                	lcall  *(%rdi)
  401a2d:	01 00                	add    %eax,(%rax)
  401a2f:	00 00                	add    %al,(%rax)
  401a31:	42 0e                	rex.X (bad) 
  401a33:	10 41 0e             	adc    %al,0xe(%rcx)
  401a36:	18 47 0e             	sbb    %al,0xe(%rdi)
  401a39:	90                   	nop
  401a3a:	02 83 03 8e 02 00    	add    0x28e03(%rbx),%al
  401a40:	14 00                	adc    $0x0,%al
  401a42:	00 00                	add    %al,(%rax)
  401a44:	1c 01                	sbb    $0x1,%al
  401a46:	00 00                	add    %al,(%rax)
  401a48:	08 f3                	or     %dh,%bl
  401a4a:	ff                   	(bad)  
  401a4b:	ff 5a 00             	lcall  *0x0(%rdx)
  401a4e:	00 00                	add    %al,(%rax)
  401a50:	00 41 0e             	add    %al,0xe(%rcx)
  401a53:	10 00                	adc    %al,(%rax)
  401a55:	00 00                	add    %al,(%rax)
  401a57:	00 44 00 00          	add    %al,0x0(%rax,%rax,1)
  401a5b:	00 34 01             	add    %dh,(%rcx,%rax,1)
  401a5e:	00 00                	add    %al,(%rax)
  401a60:	d0 fc                	sar    %ah
  401a62:	ff                   	(bad)  
  401a63:	ff 65 00             	jmpq   *0x0(%rbp)
  401a66:	00 00                	add    %al,(%rax)
  401a68:	00 42 0e             	add    %al,0xe(%rdx)
  401a6b:	10 8f 02 42 0e 18    	adc    %cl,0x180e4202(%rdi)
  401a71:	8e 03                	mov    (%rbx),%es
  401a73:	45 0e                	rex.RB (bad) 
  401a75:	20 8d 04 42 0e 28    	and    %cl,0x280e4204(%rbp)
  401a7b:	8c 05 48 0e 30 86    	mov    %es,-0x79cff1b8(%rip)        # ffffffff867028c9 <_end+0xffffffff861003f1>
  401a81:	06                   	(bad)  
  401a82:	48 0e                	rex.W (bad) 
  401a84:	38 83 07 4d 0e 40    	cmp    %al,0x400e4d07(%rbx)
  401a8a:	72 0e                	jb     401a9a <__GNU_EH_FRAME_HDR+0x1fe>
  401a8c:	38 41 0e             	cmp    %al,0xe(%rcx)
  401a8f:	30 41 0e             	xor    %al,0xe(%rcx)
  401a92:	28 42 0e             	sub    %al,0xe(%rdx)
  401a95:	20 42 0e             	and    %al,0xe(%rdx)
  401a98:	18 42 0e             	sbb    %al,0xe(%rdx)
  401a9b:	10 42 0e             	adc    %al,0xe(%rdx)
  401a9e:	08 00                	or     %al,(%rax)
  401aa0:	14 00                	adc    $0x0,%al
  401aa2:	00 00                	add    %al,(%rax)
  401aa4:	7c 01                	jl     401aa7 <__GNU_EH_FRAME_HDR+0x20b>
  401aa6:	00 00                	add    %al,(%rax)
  401aa8:	f8                   	clc    
  401aa9:	fc                   	cld    
  401aaa:	ff                   	(bad)  
  401aab:	ff 02                	incl   (%rdx)
	...

0000000000401ab8 <__FRAME_END__>:
  401ab8:	00 00                	add    %al,(%rax)
	...

Disassembly of section .gcc_except_table:

0000000000401abc <GCC_except_table0>:
  401abc:	ff 03                	incl   (%rbx)
  401abe:	92                   	xchg   %eax,%edx
  401abf:	81 80 00 03 8f 01 43 	addl   $0x43,0x18f0300(%rax)
  401ac6:	00 00 00 
  401ac9:	63 00                	movslq (%rax),%eax
  401acb:	00 00                	add    %al,(%rax)
  401acd:	a2 05 00 00 00 e8 00 	movabs %al,0xe800000005
  401ad4:	00 00 
  401ad6:	e1 01                	loope  401ad9 <GCC_except_table0+0x1d>
  401ad8:	00 00                	add    %al,(%rax)
  401ada:	b9 05 00 00 00       	mov    $0x5,%ecx
  401adf:	24 03                	and    $0x3,%al
  401ae1:	00 00                	add    %al,(%rax)
  401ae3:	0a 00                	or     (%rax),%al
  401ae5:	00 00                	add    %al,(%rax)
  401ae7:	03 05 00 00 00 3c    	add    0x3c000000(%rip),%eax        # 3c401aed <_end+0x3bdff615>
  401aed:	03 00                	add    (%rax),%eax
  401aef:	00 08                	add    %cl,(%rax)
  401af1:	00 00                	add    %al,(%rax)
  401af3:	00 46 05             	add    %al,0x5(%rsi)
  401af6:	00 00                	add    %al,(%rax)
  401af8:	00 b3 03 00 00 08    	add    %dh,0x8000003(%rbx)
  401afe:	00 00                	add    %al,(%rax)
  401b00:	00 4b 05             	add    %cl,0x5(%rbx)
  401b03:	00 00                	add    %al,(%rax)
  401b05:	00 e7                	add    %ah,%bh
  401b07:	03 00                	add    (%rax),%eax
  401b09:	00 12                	add    %dl,(%rdx)
  401b0b:	00 00                	add    %al,(%rax)
  401b0d:	00 03                	add    %al,(%rbx)
  401b0f:	05 00 00 00 09       	add    $0x9000000,%eax
  401b14:	04 00                	add    $0x0,%al
  401b16:	00 08                	add    %cl,(%rax)
  401b18:	00 00                	add    %al,(%rax)
  401b1a:	00 7c 05 00          	add    %bh,0x0(%rbp,%rax,1)
  401b1e:	00 00                	add    %al,(%rax)
  401b20:	35 04 00 00 68       	xor    $0x68000004,%eax
  401b25:	00 00                	add    %al,(%rax)
  401b27:	00 01                	add    %al,(%rcx)
  401b29:	05 00 00 00 be       	add    $0xbe000000,%eax
  401b2e:	04 00                	add    $0x0,%al
  401b30:	00 05 00 00 00 c5    	add    %al,-0x3b000000(%rip)        # ffffffffc5401b36 <_end+0xffffffffc4dff65e>
  401b36:	04 00                	add    $0x0,%al
  401b38:	00 00                	add    %al,(%rax)
  401b3a:	c3                   	retq   
  401b3b:	04 00                	add    $0x0,%al
  401b3d:	00 ec                	add    %ch,%ah
	...
  401b47:	af                   	scas   %es:(%rdi),%eax
  401b48:	05 00 00 05 00       	add    $0x50000,%eax
  401b4d:	00 00                	add    %al,(%rax)
  401b4f:	b9 05 00 00 00       	mov    $0x5,%ecx

Disassembly of section .init_array:

0000000000602000 <__frame_dummy_init_array_entry>:
  602000:	80 0e 40             	orb    $0x40,(%rsi)
  602003:	00 00                	add    %al,(%rax)
  602005:	00 00                	add    %al,(%rax)
  602007:	00 50 0d             	add    %dl,0xd(%rax)
  60200a:	40 00 00             	add    %al,(%rax)
  60200d:	00 00                	add    %al,(%rax)
	...

Disassembly of section .fini_array:

0000000000602010 <__do_global_dtors_aux_fini_array_entry>:
  602010:	60                   	(bad)  
  602011:	0e                   	(bad)  
  602012:	40 00 00             	add    %al,(%rax)
  602015:	00 00                	add    %al,(%rax)
	...

Disassembly of section .jcr:

0000000000602018 <__JCR_END__>:
	...

Disassembly of section .dynamic:

0000000000602020 <_DYNAMIC>:
  602020:	01 00                	add    %eax,(%rax)
  602022:	00 00                	add    %al,(%rax)
  602024:	00 00                	add    %al,(%rax)
  602026:	00 00                	add    %al,(%rax)
  602028:	01 00                	add    %eax,(%rax)
  60202a:	00 00                	add    %al,(%rax)
  60202c:	00 00                	add    %al,(%rax)
  60202e:	00 00                	add    %al,(%rax)
  602030:	01 00                	add    %eax,(%rax)
  602032:	00 00                	add    %al,(%rax)
  602034:	00 00                	add    %al,(%rax)
  602036:	00 00                	add    %al,(%rax)
  602038:	77 00                	ja     60203a <_DYNAMIC+0x1a>
  60203a:	00 00                	add    %al,(%rax)
  60203c:	00 00                	add    %al,(%rax)
  60203e:	00 00                	add    %al,(%rax)
  602040:	01 00                	add    %eax,(%rax)
  602042:	00 00                	add    %al,(%rax)
  602044:	00 00                	add    %al,(%rax)
  602046:	00 00                	add    %al,(%rax)
  602048:	69 00 00 00 00 00    	imul   $0x0,(%rax),%eax
  60204e:	00 00                	add    %al,(%rax)
  602050:	0c 00                	or     $0x0,%al
  602052:	00 00                	add    %al,(%rax)
  602054:	00 00                	add    %al,(%rax)
  602056:	00 00                	add    %al,(%rax)
  602058:	d8 0b                	fmuls  (%rbx)
  60205a:	40 00 00             	add    %al,(%rax)
  60205d:	00 00                	add    %al,(%rax)
  60205f:	00 0d 00 00 00 00    	add    %cl,0x0(%rip)        # 602065 <_DYNAMIC+0x45>
  602065:	00 00                	add    %al,(%rax)
  602067:	00 a4 17 40 00 00 00 	add    %ah,0x40(%rdi,%rdx,1)
  60206e:	00 00                	add    %al,(%rax)
  602070:	19 00                	sbb    %eax,(%rax)
  602072:	00 00                	add    %al,(%rax)
  602074:	00 00                	add    %al,(%rax)
  602076:	00 00                	add    %al,(%rax)
  602078:	00 20                	add    %ah,(%rax)
  60207a:	60                   	(bad)  
  60207b:	00 00                	add    %al,(%rax)
  60207d:	00 00                	add    %al,(%rax)
  60207f:	00 1b                	add    %bl,(%rbx)
  602081:	00 00                	add    %al,(%rax)
  602083:	00 00                	add    %al,(%rax)
  602085:	00 00                	add    %al,(%rax)
  602087:	00 10                	add    %dl,(%rax)
  602089:	00 00                	add    %al,(%rax)
  60208b:	00 00                	add    %al,(%rax)
  60208d:	00 00                	add    %al,(%rax)
  60208f:	00 1a                	add    %bl,(%rdx)
  602091:	00 00                	add    %al,(%rax)
  602093:	00 00                	add    %al,(%rax)
  602095:	00 00                	add    %al,(%rax)
  602097:	00 10                	add    %dl,(%rax)
  602099:	20 60 00             	and    %ah,0x0(%rax)
  60209c:	00 00                	add    %al,(%rax)
  60209e:	00 00                	add    %al,(%rax)
  6020a0:	1c 00                	sbb    $0x0,%al
  6020a2:	00 00                	add    %al,(%rax)
  6020a4:	00 00                	add    %al,(%rax)
  6020a6:	00 00                	add    %al,(%rax)
  6020a8:	08 00                	or     %al,(%rax)
  6020aa:	00 00                	add    %al,(%rax)
  6020ac:	00 00                	add    %al,(%rax)
  6020ae:	00 00                	add    %al,(%rax)
  6020b0:	04 00                	add    $0x0,%al
  6020b2:	00 00                	add    %al,(%rax)
  6020b4:	00 00                	add    %al,(%rax)
  6020b6:	00 00                	add    %al,(%rax)
  6020b8:	40 02 40 00          	add    0x0(%rax),%al
  6020bc:	00 00                	add    %al,(%rax)
  6020be:	00 00                	add    %al,(%rax)
  6020c0:	05 00 00 00 00       	add    $0x0,%eax
  6020c5:	00 00                	add    %al,(%rax)
  6020c7:	00 80 05 40 00 00    	add    %al,0x4005(%rax)
  6020cd:	00 00                	add    %al,(%rax)
  6020cf:	00 06                	add    %al,(%rsi)
  6020d1:	00 00                	add    %al,(%rax)
  6020d3:	00 00                	add    %al,(%rax)
  6020d5:	00 00                	add    %al,(%rax)
  6020d7:	00 f8                	add    %bh,%al
  6020d9:	02 40 00             	add    0x0(%rax),%al
  6020dc:	00 00                	add    %al,(%rax)
  6020de:	00 00                	add    %al,(%rax)
  6020e0:	0a 00                	or     (%rax),%al
  6020e2:	00 00                	add    %al,(%rax)
  6020e4:	00 00                	add    %al,(%rax)
  6020e6:	00 00                	add    %al,(%rax)
  6020e8:	4a 03 00             	rex.WX add (%rax),%rax
  6020eb:	00 00                	add    %al,(%rax)
  6020ed:	00 00                	add    %al,(%rax)
  6020ef:	00 0b                	add    %cl,(%rbx)
  6020f1:	00 00                	add    %al,(%rax)
  6020f3:	00 00                	add    %al,(%rax)
  6020f5:	00 00                	add    %al,(%rax)
  6020f7:	00 18                	add    %bl,(%rax)
  6020f9:	00 00                	add    %al,(%rax)
  6020fb:	00 00                	add    %al,(%rax)
  6020fd:	00 00                	add    %al,(%rax)
  6020ff:	00 15 00 00 00 00    	add    %dl,0x0(%rip)        # 602105 <_DYNAMIC+0xe5>
	...
  60210d:	00 00                	add    %al,(%rax)
  60210f:	00 03                	add    %al,(%rbx)
  602111:	00 00                	add    %al,(%rax)
  602113:	00 00                	add    %al,(%rax)
  602115:	00 00                	add    %al,(%rax)
  602117:	00 18                	add    %bl,(%rax)
  602119:	22 60 00             	and    0x0(%rax),%ah
  60211c:	00 00                	add    %al,(%rax)
  60211e:	00 00                	add    %al,(%rax)
  602120:	02 00                	add    (%rax),%al
  602122:	00 00                	add    %al,(%rax)
  602124:	00 00                	add    %al,(%rax)
  602126:	00 00                	add    %al,(%rax)
  602128:	c8 01 00 00          	enterq $0x1,$0x0
  60212c:	00 00                	add    %al,(%rax)
  60212e:	00 00                	add    %al,(%rax)
  602130:	14 00                	adc    $0x0,%al
  602132:	00 00                	add    %al,(%rax)
  602134:	00 00                	add    %al,(%rax)
  602136:	00 00                	add    %al,(%rax)
  602138:	07                   	(bad)  
  602139:	00 00                	add    %al,(%rax)
  60213b:	00 00                	add    %al,(%rax)
  60213d:	00 00                	add    %al,(%rax)
  60213f:	00 17                	add    %dl,(%rdi)
  602141:	00 00                	add    %al,(%rax)
  602143:	00 00                	add    %al,(%rax)
  602145:	00 00                	add    %al,(%rax)
  602147:	00 10                	add    %dl,(%rax)
  602149:	0a 40 00             	or     0x0(%rax),%al
  60214c:	00 00                	add    %al,(%rax)
  60214e:	00 00                	add    %al,(%rax)
  602150:	07                   	(bad)  
  602151:	00 00                	add    %al,(%rax)
  602153:	00 00                	add    %al,(%rax)
  602155:	00 00                	add    %al,(%rax)
  602157:	00 b0 09 40 00 00    	add    %dh,0x4009(%rax)
  60215d:	00 00                	add    %al,(%rax)
  60215f:	00 08                	add    %cl,(%rax)
  602161:	00 00                	add    %al,(%rax)
  602163:	00 00                	add    %al,(%rax)
  602165:	00 00                	add    %al,(%rax)
  602167:	00 60 00             	add    %ah,0x0(%rax)
  60216a:	00 00                	add    %al,(%rax)
  60216c:	00 00                	add    %al,(%rax)
  60216e:	00 00                	add    %al,(%rax)
  602170:	09 00                	or     %eax,(%rax)
  602172:	00 00                	add    %al,(%rax)
  602174:	00 00                	add    %al,(%rax)
  602176:	00 00                	add    %al,(%rax)
  602178:	18 00                	sbb    %al,(%rax)
  60217a:	00 00                	add    %al,(%rax)
  60217c:	00 00                	add    %al,(%rax)
  60217e:	00 00                	add    %al,(%rax)
  602180:	fe                   	(bad)  
  602181:	ff                   	(bad)  
  602182:	ff 6f 00             	ljmp   *0x0(%rdi)
  602185:	00 00                	add    %al,(%rax)
  602187:	00 00                	add    %al,(%rax)
  602189:	09 40 00             	or     %eax,0x0(%rax)
  60218c:	00 00                	add    %al,(%rax)
  60218e:	00 00                	add    %al,(%rax)
  602190:	ff                   	(bad)  
  602191:	ff                   	(bad)  
  602192:	ff 6f 00             	ljmp   *0x0(%rdi)
  602195:	00 00                	add    %al,(%rax)
  602197:	00 03                	add    %al,(%rbx)
  602199:	00 00                	add    %al,(%rax)
  60219b:	00 00                	add    %al,(%rax)
  60219d:	00 00                	add    %al,(%rax)
  60219f:	00 f0                	add    %dh,%al
  6021a1:	ff                   	(bad)  
  6021a2:	ff 6f 00             	ljmp   *0x0(%rdi)
  6021a5:	00 00                	add    %al,(%rax)
  6021a7:	00 ca                	add    %cl,%dl
  6021a9:	08 40 00             	or     %al,0x0(%rax)
	...

Disassembly of section .got:

0000000000602210 <.got>:
	...

Disassembly of section .got.plt:

0000000000602218 <_GLOBAL_OFFSET_TABLE_>:
  602218:	20 20                	and    %ah,(%rax)
  60221a:	60                   	(bad)  
	...
  60222f:	00 16                	add    %dl,(%rsi)
  602231:	0c 40                	or     $0x40,%al
  602233:	00 00                	add    %al,(%rax)
  602235:	00 00                	add    %al,(%rax)
  602237:	00 26                	add    %ah,(%rsi)
  602239:	0c 40                	or     $0x40,%al
  60223b:	00 00                	add    %al,(%rax)
  60223d:	00 00                	add    %al,(%rax)
  60223f:	00 36                	add    %dh,(%rsi)
  602241:	0c 40                	or     $0x40,%al
  602243:	00 00                	add    %al,(%rax)
  602245:	00 00                	add    %al,(%rax)
  602247:	00 46 0c             	add    %al,0xc(%rsi)
  60224a:	40 00 00             	add    %al,(%rax)
  60224d:	00 00                	add    %al,(%rax)
  60224f:	00 56 0c             	add    %dl,0xc(%rsi)
  602252:	40 00 00             	add    %al,(%rax)
  602255:	00 00                	add    %al,(%rax)
  602257:	00 66 0c             	add    %ah,0xc(%rsi)
  60225a:	40 00 00             	add    %al,(%rax)
  60225d:	00 00                	add    %al,(%rax)
  60225f:	00 76 0c             	add    %dh,0xc(%rsi)
  602262:	40 00 00             	add    %al,(%rax)
  602265:	00 00                	add    %al,(%rax)
  602267:	00 86 0c 40 00 00    	add    %al,0x400c(%rsi)
  60226d:	00 00                	add    %al,(%rax)
  60226f:	00 96 0c 40 00 00    	add    %dl,0x400c(%rsi)
  602275:	00 00                	add    %al,(%rax)
  602277:	00 a6 0c 40 00 00    	add    %ah,0x400c(%rsi)
  60227d:	00 00                	add    %al,(%rax)
  60227f:	00 b6 0c 40 00 00    	add    %dh,0x400c(%rsi)
  602285:	00 00                	add    %al,(%rax)
  602287:	00 c6                	add    %al,%dh
  602289:	0c 40                	or     $0x40,%al
  60228b:	00 00                	add    %al,(%rax)
  60228d:	00 00                	add    %al,(%rax)
  60228f:	00 d6                	add    %dl,%dh
  602291:	0c 40                	or     $0x40,%al
  602293:	00 00                	add    %al,(%rax)
  602295:	00 00                	add    %al,(%rax)
  602297:	00 e6                	add    %ah,%dh
  602299:	0c 40                	or     $0x40,%al
  60229b:	00 00                	add    %al,(%rax)
  60229d:	00 00                	add    %al,(%rax)
  60229f:	00 f6                	add    %dh,%dh
  6022a1:	0c 40                	or     $0x40,%al
  6022a3:	00 00                	add    %al,(%rax)
  6022a5:	00 00                	add    %al,(%rax)
  6022a7:	00 06                	add    %al,(%rsi)
  6022a9:	0d 40 00 00 00       	or     $0x40,%eax
  6022ae:	00 00                	add    %al,(%rax)
  6022b0:	16                   	(bad)  
  6022b1:	0d 40 00 00 00       	or     $0x40,%eax
  6022b6:	00 00                	add    %al,(%rax)
  6022b8:	26 0d 40 00 00 00    	es or  $0x40,%eax
  6022be:	00 00                	add    %al,(%rax)
  6022c0:	36 0d 40 00 00 00    	ss or  $0x40,%eax
	...

Disassembly of section .data:

00000000006022c8 <__data_start>:
	...

00000000006022d0 <__dso_handle>:
	...

Disassembly of section .bss:

00000000006022e0 <_ZTVN10__cxxabiv117__class_type_infoE@@CXXABI_1.3>:
	...

0000000000602340 <_ZSt4cout@@GLIBCXX_3.4>:
	...

0000000000602450 <_ZTVN10__cxxabiv120__si_class_type_infoE@@CXXABI_1.3>:
	...

00000000006024a8 <completed.7585>:
	...

00000000006024b0 <_ZL9SEPARATOR>:
	...

00000000006024d0 <_ZStL8__ioinit>:
	...

Disassembly of section .comment:

0000000000000000 <.comment>:
   0:	47                   	rex.RXB
   1:	43                   	rex.XB
   2:	43 3a 20             	rex.XB cmp (%r8),%spl
   5:	28 55 62             	sub    %dl,0x62(%rbp)
   8:	75 6e                	jne    78 <_init-0x400b60>
   a:	74 75                	je     81 <_init-0x400b57>
   c:	20 35 2e 34 2e 30    	and    %dh,0x302e342e(%rip)        # 302e3440 <_end+0x2fce0f68>
  12:	2d 36 75 62 75       	sub    $0x75627536,%eax
  17:	6e                   	outsb  %ds:(%rsi),(%dx)
  18:	74 75                	je     8f <_init-0x400b49>
  1a:	31 7e 31             	xor    %edi,0x31(%rsi)
  1d:	36 2e 30 34 2e       	ss xor %dh,%cs:(%rsi,%rbp,1)
  22:	34 29                	xor    $0x29,%al
  24:	20 35 2e 34 2e 30    	and    %dh,0x302e342e(%rip)        # 302e3458 <_end+0x2fce0f80>
  2a:	20 32                	and    %dh,(%rdx)
  2c:	30 31                	xor    %dh,(%rcx)
  2e:	36 30 36             	xor    %dh,%ss:(%rsi)
  31:	30 39                	xor    %bh,(%rcx)
  33:	00 55 62             	add    %dl,0x62(%rbp)
  36:	75 6e                	jne    a6 <_init-0x400b32>
  38:	74 75                	je     af <_init-0x400b29>
  3a:	20 63 6c             	and    %ah,0x6c(%rbx)
  3d:	61                   	(bad)  
  3e:	6e                   	outsb  %ds:(%rsi),(%dx)
  3f:	67 20 76 65          	and    %dh,0x65(%esi)
  43:	72 73                	jb     b8 <_init-0x400b20>
  45:	69 6f 6e 20 33 2e 37 	imul   $0x372e3320,0x6e(%rdi),%ebp
  4c:	2e 31 2d 32 75 62 75 	xor    %ebp,%cs:0x75627532(%rip)        # 75627585 <_end+0x750250ad>
  53:	6e                   	outsb  %ds:(%rsi),(%dx)
  54:	74 75                	je     cb <_init-0x400b0d>
  56:	32 20                	xor    (%rax),%ah
  58:	28 74 61 67          	sub    %dh,0x67(%rcx,%riz,2)
  5c:	73 2f                	jae    8d <_init-0x400b4b>
  5e:	52                   	push   %rdx
  5f:	45                   	rex.RB
  60:	4c                   	rex.WR
  61:	45                   	rex.RB
  62:	41 53                	push   %r11
  64:	45 5f                	rex.RB pop %r15
  66:	33 37                	xor    (%rdi),%esi
  68:	31 2f                	xor    %ebp,(%rdi)
  6a:	66 69 6e 61 6c 29    	imul   $0x296c,0x61(%rsi),%bp
  70:	20 28                	and    %ch,(%rax)
  72:	62 61                	(bad)  {%k7}
  74:	73 65                	jae    db <_init-0x400afd>
  76:	64 20 6f 6e          	and    %ch,%fs:0x6e(%rdi)
  7a:	20 4c 4c 56          	and    %cl,0x56(%rsp,%rcx,2)
  7e:	4d 20 33             	rex.WRB and %r14b,(%r11)
  81:	2e 37                	cs (bad) 
  83:	2e 31 29             	xor    %ebp,%cs:(%rcx)
	...
