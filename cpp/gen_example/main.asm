
main:     file format elf64-x86-64


Disassembly of section .interp:

0000000000400200 <.interp>:
  400200:	2f                   	(bad)  
  400201:	6c                   	insb   (%dx),%es:(%rdi)
  400202:	69 62 36 34 2f 6c 64 	imul   $0x646c2f34,0x36(%rdx),%esp
  400209:	2d 6c 69 6e 75       	sub    $0x756e696c,%eax
  40020e:	78 2d                	js     40023d <_init-0x813>
  400210:	78 38                	js     40024a <_init-0x806>
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
  400244:	18 00                	sbb    %al,(%rax)
  400246:	00 00                	add    %al,(%rax)
  400248:	0f 00 00             	sldt   (%rax)
  40024b:	00 12                	add    %dl,(%rdx)
  40024d:	00 00                	add    %al,(%rax)
  40024f:	00 08                	add    %cl,(%rax)
  400251:	00 00                	add    %al,(%rax)
  400253:	00 17                	add    %dl,(%rdi)
  400255:	00 00                	add    %al,(%rax)
  400257:	00 14 00             	add    %dl,(%rax,%rax,1)
  40025a:	00 00                	add    %al,(%rax)
  40025c:	10 00                	adc    %al,(%rax)
  40025e:	00 00                	add    %al,(%rax)
  400260:	16                   	(bad)  
  400261:	00 00                	add    %al,(%rax)
  400263:	00 09                	add    %cl,(%rcx)
  400265:	00 00                	add    %al,(%rax)
  400267:	00 06                	add    %al,(%rsi)
	...
  400271:	00 00                	add    %al,(%rax)
  400273:	00 11                	add    %dl,(%rcx)
  400275:	00 00                	add    %al,(%rax)
  400277:	00 0c 00             	add    %cl,(%rax,%rax,1)
  40027a:	00 00                	add    %al,(%rax)
  40027c:	0b 00                	or     (%rax),%eax
  40027e:	00 00                	add    %al,(%rax)
  400280:	0a 00                	or     (%rax),%al
  400282:	00 00                	add    %al,(%rax)
  400284:	03 00                	add    (%rax),%eax
  400286:	00 00                	add    %al,(%rax)
  400288:	01 00                	add    %eax,(%rax)
	...
  4002aa:	00 00                	add    %al,(%rax)
  4002ac:	02 00                	add    (%rax),%al
  4002ae:	00 00                	add    %al,(%rax)
  4002b0:	05 00 00 00 00       	add    $0x0,%eax
  4002b5:	00 00                	add    %al,(%rax)
  4002b7:	00 00                	add    %al,(%rax)
  4002b9:	00 00                	add    %al,(%rax)
  4002bb:	00 07                	add    %al,(%rdi)
  4002bd:	00 00                	add    %al,(%rax)
  4002bf:	00 04 00             	add    %al,(%rax,%rax,1)
	...
  4002d6:	00 00                	add    %al,(%rax)
  4002d8:	0d 00 00 00 00       	or     $0x0,%eax
  4002dd:	00 00                	add    %al,(%rax)
  4002df:	00 13                	add    %dl,(%rbx)
  4002e1:	00 00                	add    %al,(%rax)
  4002e3:	00 0e                	add    %cl,(%rsi)
  4002e5:	00 00                	add    %al,(%rax)
  4002e7:	00                   	.byte 0x0
  4002e8:	15                   	.byte 0x15
  4002e9:	00 00                	add    %al,(%rax)
	...

Disassembly of section .dynsym:

00000000004002f0 <.dynsym>:
	...
  400308:	65 01 00             	add    %eax,%gs:(%rax)
  40030b:	00 12                	add    %dl,(%rdx)
	...
  40031d:	00 00                	add    %al,(%rax)
  40031f:	00 ce                	add    %cl,%dh
  400321:	00 00                	add    %al,(%rax)
  400323:	00 12                	add    %dl,(%rdx)
	...
  400335:	00 00                	add    %al,(%rax)
  400337:	00 10                	add    %dl,(%rax)
  400339:	00 00                	add    %al,(%rax)
  40033b:	00 20                	add    %ah,(%rax)
	...
  40034d:	00 00                	add    %al,(%rax)
  40034f:	00 1f                	add    %bl,(%rdi)
  400351:	00 00                	add    %al,(%rax)
  400353:	00 20                	add    %ah,(%rax)
	...
  400365:	00 00                	add    %al,(%rax)
  400367:	00 f9                	add    %bh,%cl
  400369:	01 00                	add    %eax,(%rax)
  40036b:	00 12                	add    %dl,(%rdx)
	...
  40037d:	00 00                	add    %al,(%rax)
  40037f:	00 93 00 00 00 12    	add    %dl,0x12000000(%rbx)
	...
  400395:	00 00                	add    %al,(%rax)
  400397:	00 e7                	add    %ah,%bh
  400399:	00 00                	add    %al,(%rax)
  40039b:	00 12                	add    %dl,(%rdx)
	...
  4003ad:	00 00                	add    %al,(%rax)
  4003af:	00 96 01 00 00 11    	add    %dl,0x11000001(%rsi)
  4003b5:	00 1a                	add    %bl,(%rdx)
  4003b7:	00 00                	add    %al,(%rax)
  4003b9:	1a 60 00             	sbb    0x0(%rax),%ah
  4003bc:	00 00                	add    %al,(%rax)
  4003be:	00 00                	add    %al,(%rax)
  4003c0:	10 01                	adc    %al,(%rcx)
  4003c2:	00 00                	add    %al,(%rax)
  4003c4:	00 00                	add    %al,(%rax)
  4003c6:	00 00                	add    %al,(%rax)
  4003c8:	81 00 00 00 12 00    	addl   $0x120000,(%rax)
	...
  4003de:	00 00                	add    %al,(%rax)
  4003e0:	06                   	(bad)  
  4003e1:	02 00                	add    (%rax),%al
  4003e3:	00 12                	add    %dl,(%rdx)
	...
  4003f5:	00 00                	add    %al,(%rax)
  4003f7:	00 ff                	add    %bh,%bh
  4003f9:	00 00                	add    %al,(%rax)
  4003fb:	00 12                	add    %dl,(%rdx)
  4003fd:	00 00                	add    %al,(%rax)
  4003ff:	00 f0                	add    %dh,%al
  400401:	0a 40 00             	or     0x0(%rax),%al
	...
  400410:	33 00                	xor    (%rax),%eax
  400412:	00 00                	add    %al,(%rax)
  400414:	20 00                	and    %al,(%rax)
	...
  400426:	00 00                	add    %al,(%rax)
  400428:	4f 00 00             	rex.WRXB add %r8b,(%r8)
  40042b:	00 20                	add    %ah,(%rax)
	...
  40043d:	00 00                	add    %al,(%rax)
  40043f:	00 aa 01 00 00 21    	add    %ch,0x21000001(%rdx)
  400445:	00 1a                	add    %bl,(%rdx)
  400447:	00 10                	add    %dl,(%rax)
  400449:	1b 60 00             	sbb    0x0(%rax),%esp
  40044c:	00 00                	add    %al,(%rax)
  40044e:	00 00                	add    %al,(%rax)
  400450:	58                   	pop    %rax
  400451:	00 00                	add    %al,(%rax)
  400453:	00 00                	add    %al,(%rax)
  400455:	00 00                	add    %al,(%rax)
  400457:	00 a0 01 00 00 11    	add    %ah,0x11000001(%rax)
  40045d:	00 1a                	add    %bl,(%rdx)
  40045f:	00 80 1b 60 00 00    	add    %al,0x601b(%rax)
  400465:	00 00                	add    %al,(%rax)
  400467:	00 10                	add    %dl,(%rax)
  400469:	01 00                	add    %eax,(%rax)
  40046b:	00 00                	add    %al,(%rax)
  40046d:	00 00                	add    %al,(%rax)
  40046f:	00 17                	add    %dl,(%rdi)
  400471:	01 00                	add    %eax,(%rax)
  400473:	00 12                	add    %dl,(%rdx)
	...
  400485:	00 00                	add    %al,(%rax)
  400487:	00 c0                	add    %al,%al
  400489:	00 00                	add    %al,(%rax)
  40048b:	00 12                	add    %dl,(%rdx)
	...
  40049d:	00 00                	add    %al,(%rax)
  40049f:	00 d0                	add    %dl,%al
  4004a1:	01 00                	add    %eax,(%rax)
  4004a3:	00 21                	add    %ah,(%rcx)
  4004a5:	00 1a                	add    %bl,(%rdx)
  4004a7:	00 90 1c 60 00 00    	add    %dl,0x601c(%rax)
  4004ad:	00 00                	add    %al,(%rax)
  4004af:	00 58 00             	add    %bl,0x0(%rax)
  4004b2:	00 00                	add    %al,(%rax)
  4004b4:	00 00                	add    %al,(%rax)
  4004b6:	00 00                	add    %al,(%rax)
  4004b8:	7d 01                	jge    4004bb <_init-0x595>
  4004ba:	00 00                	add    %al,(%rax)
  4004bc:	12 00                	adc    (%rax),%al
	...
  4004ce:	00 00                	add    %al,(%rax)
  4004d0:	13 02                	adc    (%rdx),%eax
  4004d2:	00 00                	add    %al,(%rax)
  4004d4:	12 00                	adc    (%rax),%al
  4004d6:	00 00                	add    %al,(%rax)
  4004d8:	30 0b                	xor    %cl,(%rbx)
  4004da:	40 00 00             	add    %al,(%rax)
	...
  4004e9:	02 00                	add    (%rax),%al
  4004eb:	00 12                	add    %dl,(%rdx)
	...
  4004fd:	00 00                	add    %al,(%rax)
  4004ff:	00 28                	add    %ch,(%rax)
  400501:	02 00                	add    (%rax),%al
  400503:	00 12                	add    %dl,(%rdx)
	...
  400515:	00 00                	add    %al,(%rax)
  400517:	00 b4 00 00 00 12 00 	add    %dh,0x120000(%rax,%rax,1)
	...

Disassembly of section .dynstr:

0000000000400530 <.dynstr>:
  400530:	00 6c 69 62          	add    %ch,0x62(%rcx,%rbp,2)
  400534:	73 74                	jae    4005aa <_init-0x4a6>
  400536:	64 63 2b             	movslq %fs:(%rbx),%ebp
  400539:	2b 2e                	sub    (%rsi),%ebp
  40053b:	73 6f                	jae    4005ac <_init-0x4a4>
  40053d:	2e 36 00 5f 5f       	cs add %bl,%ss:0x5f(%rdi)
  400542:	67 6d                	insl   (%dx),%es:(%edi)
  400544:	6f                   	outsl  %ds:(%rsi),(%dx)
  400545:	6e                   	outsb  %ds:(%rsi),(%dx)
  400546:	5f                   	pop    %rdi
  400547:	73 74                	jae    4005bd <_init-0x493>
  400549:	61                   	(bad)  
  40054a:	72 74                	jb     4005c0 <_init-0x490>
  40054c:	5f                   	pop    %rdi
  40054d:	5f                   	pop    %rdi
  40054e:	00 5f 4a             	add    %bl,0x4a(%rdi)
  400551:	76 5f                	jbe    4005b2 <_init-0x49e>
  400553:	52                   	push   %rdx
  400554:	65 67 69 73 74 65 72 	imul   $0x6c437265,%gs:0x74(%ebx),%esi
  40055b:	43 6c 
  40055d:	61                   	(bad)  
  40055e:	73 73                	jae    4005d3 <_init-0x47d>
  400560:	65 73 00             	gs jae 400563 <_init-0x4ed>
  400563:	5f                   	pop    %rdi
  400564:	49 54                	rex.WB push %r12
  400566:	4d 5f                	rex.WRB pop %r15
  400568:	64 65 72 65          	fs gs jb 4005d1 <_init-0x47f>
  40056c:	67 69 73 74 65 72 54 	imul   $0x4d547265,0x74(%ebx),%esi
  400573:	4d 
  400574:	43 6c                	rex.XB insb (%dx),%es:(%rdi)
  400576:	6f                   	outsl  %ds:(%rsi),(%dx)
  400577:	6e                   	outsb  %ds:(%rsi),(%dx)
  400578:	65 54                	gs push %rsp
  40057a:	61                   	(bad)  
  40057b:	62                   	(bad)  
  40057c:	6c                   	insb   (%dx),%es:(%rdi)
  40057d:	65 00 5f 49          	add    %bl,%gs:0x49(%rdi)
  400581:	54                   	push   %rsp
  400582:	4d 5f                	rex.WRB pop %r15
  400584:	72 65                	jb     4005eb <_init-0x465>
  400586:	67 69 73 74 65 72 54 	imul   $0x4d547265,0x74(%ebx),%esi
  40058d:	4d 
  40058e:	43 6c                	rex.XB insb (%dx),%es:(%rdi)
  400590:	6f                   	outsl  %ds:(%rsi),(%dx)
  400591:	6e                   	outsb  %ds:(%rsi),(%dx)
  400592:	65 54                	gs push %rsp
  400594:	61                   	(bad)  
  400595:	62                   	(bad)  
  400596:	6c                   	insb   (%dx),%es:(%rdi)
  400597:	65 00 6c 69 62       	add    %ch,%gs:0x62(%rcx,%rbp,2)
  40059c:	67 63 63 5f          	movslq 0x5f(%ebx),%esp
  4005a0:	73 2e                	jae    4005d0 <_init-0x480>
  4005a2:	73 6f                	jae    400613 <_init-0x43d>
  4005a4:	2e 31 00             	xor    %eax,%cs:(%rax)
  4005a7:	6c                   	insb   (%dx),%es:(%rdi)
  4005a8:	69 62 63 2e 73 6f 2e 	imul   $0x2e6f732e,0x63(%rdx),%esp
  4005af:	36 00 5f 5f          	add    %bl,%ss:0x5f(%rdi)
  4005b3:	6c                   	insb   (%dx),%es:(%rdi)
  4005b4:	69 62 63 5f 73 74 61 	imul   $0x6174735f,0x63(%rdx),%esp
  4005bb:	72 74                	jb     400631 <_init-0x41f>
  4005bd:	5f                   	pop    %rdi
  4005be:	6d                   	insl   (%dx),%es:(%rdi)
  4005bf:	61                   	(bad)  
  4005c0:	69 6e 00 5f 5a 4e 4b 	imul   $0x4b4e5a5f,0x0(%rsi),%ebp
  4005c7:	53                   	push   %rbx
  4005c8:	74 35                	je     4005ff <_init-0x451>
  4005ca:	63 74 79 70          	movslq 0x70(%rcx,%rdi,2),%esi
  4005ce:	65 49 63 45 31       	movslq %gs:0x31(%r13),%rax
  4005d3:	33 5f 4d             	xor    0x4d(%rdi),%ebx
  4005d6:	5f                   	pop    %rdi
  4005d7:	77 69                	ja     400642 <_init-0x40e>
  4005d9:	64 65 6e             	fs outsb %gs:(%rsi),(%dx)
  4005dc:	5f                   	pop    %rdi
  4005dd:	69 6e 69 74 45 76 00 	imul   $0x764574,0x69(%rsi),%ebp
  4005e4:	5f                   	pop    %rdi
  4005e5:	5a                   	pop    %rdx
  4005e6:	4e 53                	rex.WRX push %rbx
  4005e8:	6f                   	outsl  %ds:(%rsi),(%dx)
  4005e9:	33 70 75             	xor    0x75(%rax),%esi
  4005ec:	74 45                	je     400633 <_init-0x41d>
  4005ee:	63 00                	movslq (%rax),%eax
  4005f0:	5f                   	pop    %rdi
  4005f1:	5a                   	pop    %rdx
  4005f2:	4e 53                	rex.WRX push %rbx
  4005f4:	6f                   	outsl  %ds:(%rsi),(%dx)
  4005f5:	35 66 6c 75 73       	xor    $0x73756c66,%eax
  4005fa:	68 45 76 00 5f       	pushq  $0x5f007645
  4005ff:	5a                   	pop    %rdx
  400600:	4e 53                	rex.WRX push %rbx
  400602:	6f                   	outsl  %ds:(%rsi),(%dx)
  400603:	39 5f 4d             	cmp    %ebx,0x4d(%rdi)
  400606:	5f                   	pop    %rdi
  400607:	69 6e 73 65 72 74 49 	imul   $0x49747265,0x73(%rsi),%ebp
  40060e:	6c                   	insb   (%dx),%es:(%rdi)
  40060f:	45                   	rex.RB
  400610:	45 52                	rex.RB push %r10
  400612:	53                   	push   %rbx
  400613:	6f                   	outsl  %ds:(%rsi),(%dx)
  400614:	54                   	push   %rsp
  400615:	5f                   	pop    %rdi
  400616:	00 5f 5a             	add    %bl,0x5a(%rdi)
  400619:	4e 53                	rex.WRX push %rbx
  40061b:	74 38                	je     400655 <_init-0x3fb>
  40061d:	69 6f 73 5f 62 61 73 	imul   $0x7361625f,0x73(%rdi),%ebp
  400624:	65 34 49             	gs xor $0x49,%al
  400627:	6e                   	outsb  %ds:(%rsi),(%dx)
  400628:	69 74 43 31 45 76 00 	imul   $0x5f007645,0x31(%rbx,%rax,2),%esi
  40062f:	5f 
  400630:	5a                   	pop    %rdx
  400631:	4e 53                	rex.WRX push %rbx
  400633:	74 38                	je     40066d <_init-0x3e3>
  400635:	69 6f 73 5f 62 61 73 	imul   $0x7361625f,0x73(%rdi),%ebp
  40063c:	65 34 49             	gs xor $0x49,%al
  40063f:	6e                   	outsb  %ds:(%rsi),(%dx)
  400640:	69 74 44 31 45 76 00 	imul   $0x5f007645,0x31(%rsp,%rax,2),%esi
  400647:	5f 
  400648:	5a                   	pop    %rdx
  400649:	53                   	push   %rbx
  40064a:	74 31                	je     40067d <_init-0x3d3>
  40064c:	36 5f                	ss pop %rdi
  40064e:	5f                   	pop    %rdi
  40064f:	6f                   	outsl  %ds:(%rsi),(%dx)
  400650:	73 74                	jae    4006c6 <_init-0x38a>
  400652:	72 65                	jb     4006b9 <_init-0x397>
  400654:	61                   	(bad)  
  400655:	6d                   	insl   (%dx),%es:(%rdi)
  400656:	5f                   	pop    %rdi
  400657:	69 6e 73 65 72 74 49 	imul   $0x49747265,0x73(%rsi),%ebp
  40065e:	63 53 74             	movslq 0x74(%rbx),%edx
  400661:	31 31                	xor    %esi,(%rcx)
  400663:	63 68 61             	movslq 0x61(%rax),%ebp
  400666:	72 5f                	jb     4006c7 <_init-0x389>
  400668:	74 72                	je     4006dc <_init-0x374>
  40066a:	61                   	(bad)  
  40066b:	69 74 73 49 63 45 45 	imul   $0x52454563,0x49(%rbx,%rsi,2),%esi
  400672:	52 
  400673:	53                   	push   %rbx
  400674:	74 31                	je     4006a7 <_init-0x3a9>
  400676:	33 62 61             	xor    0x61(%rdx),%esp
  400679:	73 69                	jae    4006e4 <_init-0x36c>
  40067b:	63 5f 6f             	movslq 0x6f(%rdi),%ebx
  40067e:	73 74                	jae    4006f4 <_init-0x35c>
  400680:	72 65                	jb     4006e7 <_init-0x369>
  400682:	61                   	(bad)  
  400683:	6d                   	insl   (%dx),%es:(%rdi)
  400684:	49 54                	rex.WB push %r12
  400686:	5f                   	pop    %rdi
  400687:	54                   	push   %rsp
  400688:	30 5f 45             	xor    %bl,0x45(%rdi)
  40068b:	53                   	push   %rbx
  40068c:	36 5f                	ss pop %rdi
  40068e:	50                   	push   %rax
  40068f:	4b 53                	rex.WXB push %r11
  400691:	33 5f 6c             	xor    0x6c(%rdi),%ebx
  400694:	00 5f 5a             	add    %bl,0x5a(%rdi)
  400697:	53                   	push   %rbx
  400698:	74 31                	je     4006cb <_init-0x385>
  40069a:	36 5f                	ss pop %rdi
  40069c:	5f                   	pop    %rdi
  40069d:	74 68                	je     400707 <_init-0x349>
  40069f:	72 6f                	jb     400710 <_init-0x340>
  4006a1:	77 5f                	ja     400702 <_init-0x34e>
  4006a3:	62 61 64 5f 63       	(bad)  {%k7}
  4006a8:	61                   	(bad)  
  4006a9:	73 74                	jae    40071f <_init-0x331>
  4006ab:	76 00                	jbe    4006ad <_init-0x3a3>
  4006ad:	5f                   	pop    %rdi
  4006ae:	5a                   	pop    %rdx
  4006af:	53                   	push   %rbx
  4006b0:	74 31                	je     4006e3 <_init-0x36d>
  4006b2:	37                   	(bad)  
  4006b3:	5f                   	pop    %rdi
  4006b4:	5f                   	pop    %rdi
  4006b5:	74 68                	je     40071f <_init-0x331>
  4006b7:	72 6f                	jb     400728 <_init-0x328>
  4006b9:	77 5f                	ja     40071a <_init-0x336>
  4006bb:	62 61 64 5f 61       	(bad)  {%k7}
  4006c0:	6c                   	insb   (%dx),%es:(%rdi)
  4006c1:	6c                   	insb   (%dx),%es:(%rdi)
  4006c2:	6f                   	outsl  %ds:(%rsi),(%dx)
  4006c3:	63 76 00             	movslq 0x0(%rsi),%esi
  4006c6:	5f                   	pop    %rdi
  4006c7:	5a                   	pop    %rdx
  4006c8:	53                   	push   %rbx
  4006c9:	74 34                	je     4006ff <_init-0x351>
  4006cb:	63 65 72             	movslq 0x72(%rbp),%esp
  4006ce:	72 00                	jb     4006d0 <_init-0x380>
  4006d0:	5f                   	pop    %rdi
  4006d1:	5a                   	pop    %rdx
  4006d2:	53                   	push   %rbx
  4006d3:	74 34                	je     400709 <_init-0x347>
  4006d5:	63 6f 75             	movslq 0x75(%rdi),%ebp
  4006d8:	74 00                	je     4006da <_init-0x376>
  4006da:	5f                   	pop    %rdi
  4006db:	5a                   	pop    %rdx
  4006dc:	54                   	push   %rsp
  4006dd:	56                   	push   %rsi
  4006de:	4e 31 30             	rex.WRX xor %r14,(%rax)
  4006e1:	5f                   	pop    %rdi
  4006e2:	5f                   	pop    %rdi
  4006e3:	63 78 78             	movslq 0x78(%rax),%edi
  4006e6:	61                   	(bad)  
  4006e7:	62                   	(bad)  {%k7}
  4006e8:	69 76 31 31 37 5f 5f 	imul   $0x5f5f3731,0x31(%rsi),%esi
  4006ef:	63 6c 61 73          	movslq 0x73(%rcx,%riz,2),%ebp
  4006f3:	73 5f                	jae    400754 <_init-0x2fc>
  4006f5:	74 79                	je     400770 <_init-0x2e0>
  4006f7:	70 65                	jo     40075e <_init-0x2f2>
  4006f9:	5f                   	pop    %rdi
  4006fa:	69 6e 66 6f 45 00 5f 	imul   $0x5f00456f,0x66(%rsi),%ebp
  400701:	5a                   	pop    %rdx
  400702:	54                   	push   %rsp
  400703:	56                   	push   %rsi
  400704:	4e 31 30             	rex.WRX xor %r14,(%rax)
  400707:	5f                   	pop    %rdi
  400708:	5f                   	pop    %rdi
  400709:	63 78 78             	movslq 0x78(%rax),%edi
  40070c:	61                   	(bad)  
  40070d:	62                   	(bad)  {%k7}
  40070e:	69 76 31 32 30 5f 5f 	imul   $0x5f5f3032,0x31(%rsi),%esi
  400715:	73 69                	jae    400780 <_init-0x2d0>
  400717:	5f                   	pop    %rdi
  400718:	63 6c 61 73          	movslq 0x73(%rcx,%riz,2),%ebp
  40071c:	73 5f                	jae    40077d <_init-0x2d3>
  40071e:	74 79                	je     400799 <_init-0x2b7>
  400720:	70 65                	jo     400787 <_init-0x2c9>
  400722:	5f                   	pop    %rdi
  400723:	69 6e 66 6f 45 00 5f 	imul   $0x5f00456f,0x66(%rsi),%ebp
  40072a:	5a                   	pop    %rdx
  40072b:	64 6c                	fs insb (%dx),%es:(%rdi)
  40072d:	50                   	push   %rax
  40072e:	76 00                	jbe    400730 <_init-0x320>
  400730:	5f                   	pop    %rdi
  400731:	5a                   	pop    %rdx
  400732:	6e                   	outsb  %ds:(%rsi),(%dx)
  400733:	77 6d                	ja     4007a2 <_init-0x2ae>
  400735:	00 5f 5f             	add    %bl,0x5f(%rdi)
  400738:	63 78 61             	movslq 0x61(%rax),%edi
  40073b:	5f                   	pop    %rdi
  40073c:	61                   	(bad)  
  40073d:	74 65                	je     4007a4 <_init-0x2ac>
  40073f:	78 69                	js     4007aa <_init-0x2a6>
  400741:	74 00                	je     400743 <_init-0x30d>
  400743:	5f                   	pop    %rdi
  400744:	5f                   	pop    %rdi
  400745:	67 78 78             	addr32 js 4007c0 <_init-0x290>
  400748:	5f                   	pop    %rdi
  400749:	70 65                	jo     4007b0 <_init-0x2a0>
  40074b:	72 73                	jb     4007c0 <_init-0x290>
  40074d:	6f                   	outsl  %ds:(%rsi),(%dx)
  40074e:	6e                   	outsb  %ds:(%rsi),(%dx)
  40074f:	61                   	(bad)  
  400750:	6c                   	insb   (%dx),%es:(%rdi)
  400751:	69 74 79 5f 76 30 00 	imul   $0x5f003076,0x5f(%rcx,%rdi,2),%esi
  400758:	5f 
  400759:	55                   	push   %rbp
  40075a:	6e                   	outsb  %ds:(%rsi),(%dx)
  40075b:	77 69                	ja     4007c6 <_init-0x28a>
  40075d:	6e                   	outsb  %ds:(%rsi),(%dx)
  40075e:	64 5f                	fs pop %rdi
  400760:	52                   	push   %rdx
  400761:	65 73 75             	gs jae 4007d9 <_init-0x277>
  400764:	6d                   	insl   (%dx),%es:(%rdi)
  400765:	65 00 47 43          	add    %al,%gs:0x43(%rdi)
  400769:	43 5f                	rex.XB pop %r15
  40076b:	33 2e                	xor    (%rsi),%ebp
  40076d:	30 00                	xor    %al,(%rax)
  40076f:	47                   	rex.RXB
  400770:	4c                   	rex.WR
  400771:	49                   	rex.WB
  400772:	42                   	rex.X
  400773:	43 5f                	rex.XB pop %r15
  400775:	32 2e                	xor    (%rsi),%ch
  400777:	32 2e                	xor    (%rsi),%ch
  400779:	35 00 43 58 58       	xor    $0x58584300,%eax
  40077e:	41                   	rex.B
  40077f:	42                   	rex.X
  400780:	49 5f                	rex.WB pop %r15
  400782:	31 2e                	xor    %ebp,(%rsi)
  400784:	33 00                	xor    (%rax),%eax
  400786:	47                   	rex.RXB
  400787:	4c                   	rex.WR
  400788:	49                   	rex.WB
  400789:	42                   	rex.X
  40078a:	43 58                	rex.XB pop %r8
  40078c:	58                   	pop    %rax
  40078d:	5f                   	pop    %rdi
  40078e:	33 2e                	xor    (%rsi),%ebp
  400790:	34 2e                	xor    $0x2e,%al
  400792:	31 31                	xor    %esi,(%rcx)
  400794:	00 47 4c             	add    %al,0x4c(%rdi)
  400797:	49                   	rex.WB
  400798:	42                   	rex.X
  400799:	43 58                	rex.XB pop %r8
  40079b:	58                   	pop    %rax
  40079c:	5f                   	pop    %rdi
  40079d:	33 2e                	xor    (%rsi),%ebp
  40079f:	34 2e                	xor    $0x2e,%al
  4007a1:	39 00                	cmp    %eax,(%rax)
  4007a3:	47                   	rex.RXB
  4007a4:	4c                   	rex.WR
  4007a5:	49                   	rex.WB
  4007a6:	42                   	rex.X
  4007a7:	43 58                	rex.XB pop %r8
  4007a9:	58                   	pop    %rax
  4007aa:	5f                   	pop    %rdi
  4007ab:	33 2e                	xor    (%rsi),%ebp
  4007ad:	34 00                	xor    $0x0,%al

Disassembly of section .gnu.version:

00000000004007b0 <.gnu.version>:
  4007b0:	00 00                	add    %al,(%rax)
  4007b2:	02 00                	add    (%rax),%al
  4007b4:	03 00                	add    (%rax),%eax
  4007b6:	00 00                	add    %al,(%rax)
  4007b8:	00 00                	add    %al,(%rax)
  4007ba:	02 00                	add    (%rax),%al
  4007bc:	04 00                	add    $0x0,%al
  4007be:	02 00                	add    (%rax),%al
  4007c0:	02 00                	add    (%rax),%al
  4007c2:	05 00 05 00 02       	add    $0x2000500,%eax
  4007c7:	00 00                	add    %al,(%rax)
  4007c9:	00 00                	add    %al,(%rax)
  4007cb:	00 06                	add    %al,(%rsi)
  4007cd:	00 02                	add    %al,(%rdx)
  4007cf:	00 03                	add    %al,(%rbx)
  4007d1:	00 02                	add    %al,(%rdx)
  4007d3:	00 06                	add    %al,(%rsi)
  4007d5:	00 02                	add    %al,(%rdx)
  4007d7:	00 06                	add    %al,(%rsi)
  4007d9:	00 02                	add    %al,(%rdx)
  4007db:	00 07                	add    %al,(%rdi)
  4007dd:	00 02                	add    %al,(%rdx)
	...

Disassembly of section .gnu.version_r:

00000000004007e0 <.gnu.version_r>:
  4007e0:	01 00                	add    %eax,(%rax)
  4007e2:	01 00                	add    %eax,(%rax)
  4007e4:	69 00 00 00 10 00    	imul   $0x100000,(%rax),%eax
  4007ea:	00 00                	add    %al,(%rax)
  4007ec:	20 00                	and    %al,(%rax)
  4007ee:	00 00                	add    %al,(%rax)
  4007f0:	50                   	push   %rax
  4007f1:	26 79 0b             	es jns 4007ff <_init-0x251>
  4007f4:	00 00                	add    %al,(%rax)
  4007f6:	07                   	(bad)  
  4007f7:	00 37                	add    %dh,(%rdi)
  4007f9:	02 00                	add    (%rax),%al
  4007fb:	00 00                	add    %al,(%rax)
  4007fd:	00 00                	add    %al,(%rax)
  4007ff:	00 01                	add    %al,(%rcx)
  400801:	00 01                	add    %al,(%rcx)
  400803:	00 77 00             	add    %dh,0x0(%rdi)
  400806:	00 00                	add    %al,(%rax)
  400808:	10 00                	adc    %al,(%rax)
  40080a:	00 00                	add    %al,(%rax)
  40080c:	20 00                	and    %al,(%rax)
  40080e:	00 00                	add    %al,(%rax)
  400810:	75 1a                	jne    40082c <_init-0x224>
  400812:	69 09 00 00 05 00    	imul   $0x50000,(%rcx),%ecx
  400818:	3f                   	(bad)  
  400819:	02 00                	add    (%rax),%al
  40081b:	00 00                	add    %al,(%rax)
  40081d:	00 00                	add    %al,(%rax)
  40081f:	00 01                	add    %al,(%rcx)
  400821:	00 04 00             	add    %al,(%rax,%rax,1)
  400824:	01 00                	add    %eax,(%rax)
  400826:	00 00                	add    %al,(%rax)
  400828:	10 00                	adc    %al,(%rax)
  40082a:	00 00                	add    %al,(%rax)
  40082c:	00 00                	add    %al,(%rax)
  40082e:	00 00                	add    %al,(%rax)
  400830:	d3 af 6b 05 00 00    	shrl   %cl,0x56b(%rdi)
  400836:	06                   	(bad)  
  400837:	00 4b 02             	add    %cl,0x2(%rbx)
  40083a:	00 00                	add    %al,(%rax)
  40083c:	10 00                	adc    %al,(%rax)
  40083e:	00 00                	add    %al,(%rax)
  400840:	61                   	(bad)  
  400841:	f8                   	clc    
  400842:	97                   	xchg   %eax,%edi
  400843:	02 00                	add    (%rax),%al
  400845:	00 04 00             	add    %al,(%rax,%rax,1)
  400848:	56                   	push   %rsi
  400849:	02 00                	add    (%rax),%al
  40084b:	00 10                	add    %dl,(%rax)
  40084d:	00 00                	add    %al,(%rax)
  40084f:	00 89 7f 29 02 00    	add    %cl,0x2297f(%rcx)
  400855:	00 03                	add    %al,(%rbx)
  400857:	00 65 02             	add    %ah,0x2(%rbp)
  40085a:	00 00                	add    %al,(%rax)
  40085c:	10 00                	adc    %al,(%rax)
  40085e:	00 00                	add    %al,(%rax)
  400860:	74 29                	je     40088b <_init-0x1c5>
  400862:	92                   	xchg   %eax,%edx
  400863:	08 00                	or     %al,(%rax)
  400865:	00 02                	add    %al,(%rdx)
  400867:	00 73 02             	add    %dh,0x2(%rbx)
  40086a:	00 00                	add    %al,(%rax)
  40086c:	00 00                	add    %al,(%rax)
	...

Disassembly of section .rela.dyn:

0000000000400870 <.rela.dyn>:
  400870:	58                   	pop    %rax
  400871:	19 60 00             	sbb    %esp,0x0(%rax)
  400874:	00 00                	add    %al,(%rax)
  400876:	00 00                	add    %al,(%rax)
  400878:	06                   	(bad)  
  400879:	00 00                	add    %al,(%rax)
  40087b:	00 03                	add    %al,(%rbx)
	...
  400889:	1a 60 00             	sbb    0x0(%rax),%ah
  40088c:	00 00                	add    %al,(%rax)
  40088e:	00 00                	add    %al,(%rax)
  400890:	05 00 00 00 08       	add    $0x8000000,%eax
	...
  40089d:	00 00                	add    %al,(%rax)
  40089f:	00 10                	add    %dl,(%rax)
  4008a1:	1b 60 00             	sbb    0x0(%rax),%esp
  4008a4:	00 00                	add    %al,(%rax)
  4008a6:	00 00                	add    %al,(%rax)
  4008a8:	05 00 00 00 0e       	add    $0xe000000,%eax
	...
  4008b5:	00 00                	add    %al,(%rax)
  4008b7:	00 80 1b 60 00 00    	add    %al,0x601b(%rax)
  4008bd:	00 00                	add    %al,(%rax)
  4008bf:	00 05 00 00 00 0f    	add    %al,0xf000000(%rip)        # f4008c5 <_end+0xedfebd5>
	...
  4008cd:	00 00                	add    %al,(%rax)
  4008cf:	00 90 1c 60 00 00    	add    %dl,0x601c(%rax)
  4008d5:	00 00                	add    %al,(%rax)
  4008d7:	00 05 00 00 00 12    	add    %al,0x12000000(%rip)        # 124008dd <_end+0x11dfebed>
	...

Disassembly of section .rela.plt:

00000000004008e8 <.rela.plt>:
  4008e8:	78 19                	js     400903 <_init-0x14d>
  4008ea:	60                   	(bad)  
  4008eb:	00 00                	add    %al,(%rax)
  4008ed:	00 00                	add    %al,(%rax)
  4008ef:	00 07                	add    %al,(%rdi)
  4008f1:	00 00                	add    %al,(%rax)
  4008f3:	00 01                	add    %al,(%rcx)
	...
  4008fd:	00 00                	add    %al,(%rax)
  4008ff:	00 80 19 60 00 00    	add    %al,0x6019(%rax)
  400905:	00 00                	add    %al,(%rax)
  400907:	00 07                	add    %al,(%rdi)
  400909:	00 00                	add    %al,(%rax)
  40090b:	00 02                	add    %al,(%rdx)
	...
  400915:	00 00                	add    %al,(%rax)
  400917:	00 88 19 60 00 00    	add    %cl,0x6019(%rax)
  40091d:	00 00                	add    %al,(%rax)
  40091f:	00 07                	add    %al,(%rdi)
  400921:	00 00                	add    %al,(%rax)
  400923:	00 05 00 00 00 00    	add    %al,0x0(%rip)        # 400929 <_init-0x127>
  400929:	00 00                	add    %al,(%rax)
  40092b:	00 00                	add    %al,(%rax)
  40092d:	00 00                	add    %al,(%rax)
  40092f:	00 90 19 60 00 00    	add    %dl,0x6019(%rax)
  400935:	00 00                	add    %al,(%rax)
  400937:	00 07                	add    %al,(%rdi)
  400939:	00 00                	add    %al,(%rax)
  40093b:	00 06                	add    %al,(%rsi)
	...
  400945:	00 00                	add    %al,(%rax)
  400947:	00 98 19 60 00 00    	add    %bl,0x6019(%rax)
  40094d:	00 00                	add    %al,(%rax)
  40094f:	00 07                	add    %al,(%rdi)
  400951:	00 00                	add    %al,(%rax)
  400953:	00 07                	add    %al,(%rdi)
	...
  40095d:	00 00                	add    %al,(%rax)
  40095f:	00 a0 19 60 00 00    	add    %ah,0x6019(%rax)
  400965:	00 00                	add    %al,(%rax)
  400967:	00 07                	add    %al,(%rdi)
  400969:	00 00                	add    %al,(%rax)
  40096b:	00 09                	add    %cl,(%rcx)
	...
  400975:	00 00                	add    %al,(%rax)
  400977:	00 a8 19 60 00 00    	add    %ch,0x6019(%rax)
  40097d:	00 00                	add    %al,(%rax)
  40097f:	00 07                	add    %al,(%rdi)
  400981:	00 00                	add    %al,(%rax)
  400983:	00 0a                	add    %cl,(%rdx)
	...
  40098d:	00 00                	add    %al,(%rax)
  40098f:	00 b0 19 60 00 00    	add    %dh,0x6019(%rax)
  400995:	00 00                	add    %al,(%rax)
  400997:	00 07                	add    %al,(%rdi)
  400999:	00 00                	add    %al,(%rax)
  40099b:	00 0b                	add    %cl,(%rbx)
	...
  4009a5:	00 00                	add    %al,(%rax)
  4009a7:	00 b8 19 60 00 00    	add    %bh,0x6019(%rax)
  4009ad:	00 00                	add    %al,(%rax)
  4009af:	00 07                	add    %al,(%rdi)
  4009b1:	00 00                	add    %al,(%rax)
  4009b3:	00 10                	add    %dl,(%rax)
	...
  4009bd:	00 00                	add    %al,(%rax)
  4009bf:	00 c0                	add    %al,%al
  4009c1:	19 60 00             	sbb    %esp,0x0(%rax)
  4009c4:	00 00                	add    %al,(%rax)
  4009c6:	00 00                	add    %al,(%rax)
  4009c8:	07                   	(bad)  
  4009c9:	00 00                	add    %al,(%rax)
  4009cb:	00 11                	add    %dl,(%rcx)
	...
  4009d5:	00 00                	add    %al,(%rax)
  4009d7:	00 c8                	add    %cl,%al
  4009d9:	19 60 00             	sbb    %esp,0x0(%rax)
  4009dc:	00 00                	add    %al,(%rax)
  4009de:	00 00                	add    %al,(%rax)
  4009e0:	07                   	(bad)  
  4009e1:	00 00                	add    %al,(%rax)
  4009e3:	00 13                	add    %dl,(%rbx)
	...
  4009ed:	00 00                	add    %al,(%rax)
  4009ef:	00 d0                	add    %dl,%al
  4009f1:	19 60 00             	sbb    %esp,0x0(%rax)
  4009f4:	00 00                	add    %al,(%rax)
  4009f6:	00 00                	add    %al,(%rax)
  4009f8:	07                   	(bad)  
  4009f9:	00 00                	add    %al,(%rax)
  4009fb:	00 14 00             	add    %dl,(%rax,%rax,1)
	...
  400a06:	00 00                	add    %al,(%rax)
  400a08:	d8 19                	fcomps (%rcx)
  400a0a:	60                   	(bad)  
  400a0b:	00 00                	add    %al,(%rax)
  400a0d:	00 00                	add    %al,(%rax)
  400a0f:	00 07                	add    %al,(%rdi)
  400a11:	00 00                	add    %al,(%rax)
  400a13:	00 15 00 00 00 00    	add    %dl,0x0(%rip)        # 400a19 <_init-0x37>
  400a19:	00 00                	add    %al,(%rax)
  400a1b:	00 00                	add    %al,(%rax)
  400a1d:	00 00                	add    %al,(%rax)
  400a1f:	00 e0                	add    %ah,%al
  400a21:	19 60 00             	sbb    %esp,0x0(%rax)
  400a24:	00 00                	add    %al,(%rax)
  400a26:	00 00                	add    %al,(%rax)
  400a28:	07                   	(bad)  
  400a29:	00 00                	add    %al,(%rax)
  400a2b:	00 16                	add    %dl,(%rsi)
	...
  400a35:	00 00                	add    %al,(%rax)
  400a37:	00 e8                	add    %ch,%al
  400a39:	19 60 00             	sbb    %esp,0x0(%rax)
  400a3c:	00 00                	add    %al,(%rax)
  400a3e:	00 00                	add    %al,(%rax)
  400a40:	07                   	(bad)  
  400a41:	00 00                	add    %al,(%rax)
  400a43:	00 17                	add    %dl,(%rdi)
	...

Disassembly of section .init:

0000000000400a50 <_init>:
  400a50:	48 83 ec 08          	sub    $0x8,%rsp
  400a54:	48 8b 05 fd 0e 20 00 	mov    0x200efd(%rip),%rax        # 601958 <_DYNAMIC+0x1f0>
  400a5b:	48 85 c0             	test   %rax,%rax
  400a5e:	74 05                	je     400a65 <_init+0x15>
  400a60:	e8 0b 01 00 00       	callq  400b70 <_ZNSo3putEc@plt+0x10>
  400a65:	48 83 c4 08          	add    $0x8,%rsp
  400a69:	c3                   	retq   

Disassembly of section .plt:

0000000000400a70 <_ZSt16__throw_bad_castv@plt-0x10>:
  400a70:	ff 35 f2 0e 20 00    	pushq  0x200ef2(%rip)        # 601968 <_GLOBAL_OFFSET_TABLE_+0x8>
  400a76:	ff 25 f4 0e 20 00    	jmpq   *0x200ef4(%rip)        # 601970 <_GLOBAL_OFFSET_TABLE_+0x10>
  400a7c:	0f 1f 40 00          	nopl   0x0(%rax)

0000000000400a80 <_ZSt16__throw_bad_castv@plt>:
  400a80:	ff 25 f2 0e 20 00    	jmpq   *0x200ef2(%rip)        # 601978 <_GLOBAL_OFFSET_TABLE_+0x18>
  400a86:	68 00 00 00 00       	pushq  $0x0
  400a8b:	e9 e0 ff ff ff       	jmpq   400a70 <_init+0x20>

0000000000400a90 <_ZNSo9_M_insertIlEERSoT_@plt>:
  400a90:	ff 25 ea 0e 20 00    	jmpq   *0x200eea(%rip)        # 601980 <_GLOBAL_OFFSET_TABLE_+0x20>
  400a96:	68 01 00 00 00       	pushq  $0x1
  400a9b:	e9 d0 ff ff ff       	jmpq   400a70 <_init+0x20>

0000000000400aa0 <_ZdlPv@plt>:
  400aa0:	ff 25 e2 0e 20 00    	jmpq   *0x200ee2(%rip)        # 601988 <_GLOBAL_OFFSET_TABLE_+0x28>
  400aa6:	68 02 00 00 00       	pushq  $0x2
  400aab:	e9 c0 ff ff ff       	jmpq   400a70 <_init+0x20>

0000000000400ab0 <_ZNKSt5ctypeIcE13_M_widen_initEv@plt>:
  400ab0:	ff 25 da 0e 20 00    	jmpq   *0x200eda(%rip)        # 601990 <_GLOBAL_OFFSET_TABLE_+0x30>
  400ab6:	68 03 00 00 00       	pushq  $0x3
  400abb:	e9 b0 ff ff ff       	jmpq   400a70 <_init+0x20>

0000000000400ac0 <_ZNSt8ios_base4InitC1Ev@plt>:
  400ac0:	ff 25 d2 0e 20 00    	jmpq   *0x200ed2(%rip)        # 601998 <_GLOBAL_OFFSET_TABLE_+0x38>
  400ac6:	68 04 00 00 00       	pushq  $0x4
  400acb:	e9 a0 ff ff ff       	jmpq   400a70 <_init+0x20>

0000000000400ad0 <__libc_start_main@plt>:
  400ad0:	ff 25 ca 0e 20 00    	jmpq   *0x200eca(%rip)        # 6019a0 <_GLOBAL_OFFSET_TABLE_+0x40>
  400ad6:	68 05 00 00 00       	pushq  $0x5
  400adb:	e9 90 ff ff ff       	jmpq   400a70 <_init+0x20>

0000000000400ae0 <__cxa_atexit@plt>:
  400ae0:	ff 25 c2 0e 20 00    	jmpq   *0x200ec2(%rip)        # 6019a8 <_GLOBAL_OFFSET_TABLE_+0x48>
  400ae6:	68 06 00 00 00       	pushq  $0x6
  400aeb:	e9 80 ff ff ff       	jmpq   400a70 <_init+0x20>

0000000000400af0 <_ZNSt8ios_base4InitD1Ev@plt>:
  400af0:	ff 25 ba 0e 20 00    	jmpq   *0x200eba(%rip)        # 6019b0 <_GLOBAL_OFFSET_TABLE_+0x50>
  400af6:	68 07 00 00 00       	pushq  $0x7
  400afb:	e9 70 ff ff ff       	jmpq   400a70 <_init+0x20>

0000000000400b00 <_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@plt>:
  400b00:	ff 25 b2 0e 20 00    	jmpq   *0x200eb2(%rip)        # 6019b8 <_GLOBAL_OFFSET_TABLE_+0x58>
  400b06:	68 08 00 00 00       	pushq  $0x8
  400b0b:	e9 60 ff ff ff       	jmpq   400a70 <_init+0x20>

0000000000400b10 <_ZNSo5flushEv@plt>:
  400b10:	ff 25 aa 0e 20 00    	jmpq   *0x200eaa(%rip)        # 6019c0 <_GLOBAL_OFFSET_TABLE_+0x60>
  400b16:	68 09 00 00 00       	pushq  $0x9
  400b1b:	e9 50 ff ff ff       	jmpq   400a70 <_init+0x20>

0000000000400b20 <_ZSt17__throw_bad_allocv@plt>:
  400b20:	ff 25 a2 0e 20 00    	jmpq   *0x200ea2(%rip)        # 6019c8 <_GLOBAL_OFFSET_TABLE_+0x68>
  400b26:	68 0a 00 00 00       	pushq  $0xa
  400b2b:	e9 40 ff ff ff       	jmpq   400a70 <_init+0x20>

0000000000400b30 <__gxx_personality_v0@plt>:
  400b30:	ff 25 9a 0e 20 00    	jmpq   *0x200e9a(%rip)        # 6019d0 <_GLOBAL_OFFSET_TABLE_+0x70>
  400b36:	68 0b 00 00 00       	pushq  $0xb
  400b3b:	e9 30 ff ff ff       	jmpq   400a70 <_init+0x20>

0000000000400b40 <_Znwm@plt>:
  400b40:	ff 25 92 0e 20 00    	jmpq   *0x200e92(%rip)        # 6019d8 <_GLOBAL_OFFSET_TABLE_+0x78>
  400b46:	68 0c 00 00 00       	pushq  $0xc
  400b4b:	e9 20 ff ff ff       	jmpq   400a70 <_init+0x20>

0000000000400b50 <_Unwind_Resume@plt>:
  400b50:	ff 25 8a 0e 20 00    	jmpq   *0x200e8a(%rip)        # 6019e0 <_GLOBAL_OFFSET_TABLE_+0x80>
  400b56:	68 0d 00 00 00       	pushq  $0xd
  400b5b:	e9 10 ff ff ff       	jmpq   400a70 <_init+0x20>

0000000000400b60 <_ZNSo3putEc@plt>:
  400b60:	ff 25 82 0e 20 00    	jmpq   *0x200e82(%rip)        # 6019e8 <_GLOBAL_OFFSET_TABLE_+0x88>
  400b66:	68 0e 00 00 00       	pushq  $0xe
  400b6b:	e9 00 ff ff ff       	jmpq   400a70 <_init+0x20>

Disassembly of section .plt.got:

0000000000400b70 <.plt.got>:
  400b70:	ff 25 e2 0d 20 00    	jmpq   *0x200de2(%rip)        # 601958 <_DYNAMIC+0x1f0>
  400b76:	66 90                	xchg   %ax,%ax

Disassembly of section .text:

0000000000400b80 <_GLOBAL__sub_I_execute.cpp>:
  400b80:	50                   	push   %rax
  400b81:	bf ec 1c 60 00       	mov    $0x601cec,%edi
  400b86:	e8 35 ff ff ff       	callq  400ac0 <_ZNSt8ios_base4InitC1Ev@plt>
  400b8b:	bf f0 0a 40 00       	mov    $0x400af0,%edi
  400b90:	be ec 1c 60 00       	mov    $0x601cec,%esi
  400b95:	ba f8 19 60 00       	mov    $0x6019f8,%edx
  400b9a:	58                   	pop    %rax
  400b9b:	e9 40 ff ff ff       	jmpq   400ae0 <__cxa_atexit@plt>

0000000000400ba0 <_start>:
  400ba0:	31 ed                	xor    %ebp,%ebp
  400ba2:	49 89 d1             	mov    %rdx,%r9
  400ba5:	5e                   	pop    %rsi
  400ba6:	48 89 e2             	mov    %rsp,%rdx
  400ba9:	48 83 e4 f0          	and    $0xfffffffffffffff0,%rsp
  400bad:	50                   	push   %rax
  400bae:	54                   	push   %rsp
  400baf:	49 c7 c0 c0 13 40 00 	mov    $0x4013c0,%r8
  400bb6:	48 c7 c1 50 13 40 00 	mov    $0x401350,%rcx
  400bbd:	48 c7 c7 a0 0c 40 00 	mov    $0x400ca0,%rdi
  400bc4:	e8 07 ff ff ff       	callq  400ad0 <__libc_start_main@plt>
  400bc9:	f4                   	hlt    
  400bca:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)

0000000000400bd0 <deregister_tm_clones>:
  400bd0:	b8 07 1a 60 00       	mov    $0x601a07,%eax
  400bd5:	55                   	push   %rbp
  400bd6:	48 2d 00 1a 60 00    	sub    $0x601a00,%rax
  400bdc:	48 83 f8 0e          	cmp    $0xe,%rax
  400be0:	48 89 e5             	mov    %rsp,%rbp
  400be3:	76 1b                	jbe    400c00 <deregister_tm_clones+0x30>
  400be5:	b8 00 00 00 00       	mov    $0x0,%eax
  400bea:	48 85 c0             	test   %rax,%rax
  400bed:	74 11                	je     400c00 <deregister_tm_clones+0x30>
  400bef:	5d                   	pop    %rbp
  400bf0:	bf 00 1a 60 00       	mov    $0x601a00,%edi
  400bf5:	ff e0                	jmpq   *%rax
  400bf7:	66 0f 1f 84 00 00 00 	nopw   0x0(%rax,%rax,1)
  400bfe:	00 00 
  400c00:	5d                   	pop    %rbp
  400c01:	c3                   	retq   
  400c02:	0f 1f 40 00          	nopl   0x0(%rax)
  400c06:	66 2e 0f 1f 84 00 00 	nopw   %cs:0x0(%rax,%rax,1)
  400c0d:	00 00 00 

0000000000400c10 <register_tm_clones>:
  400c10:	be 00 1a 60 00       	mov    $0x601a00,%esi
  400c15:	55                   	push   %rbp
  400c16:	48 81 ee 00 1a 60 00 	sub    $0x601a00,%rsi
  400c1d:	48 c1 fe 03          	sar    $0x3,%rsi
  400c21:	48 89 e5             	mov    %rsp,%rbp
  400c24:	48 89 f0             	mov    %rsi,%rax
  400c27:	48 c1 e8 3f          	shr    $0x3f,%rax
  400c2b:	48 01 c6             	add    %rax,%rsi
  400c2e:	48 d1 fe             	sar    %rsi
  400c31:	74 15                	je     400c48 <register_tm_clones+0x38>
  400c33:	b8 00 00 00 00       	mov    $0x0,%eax
  400c38:	48 85 c0             	test   %rax,%rax
  400c3b:	74 0b                	je     400c48 <register_tm_clones+0x38>
  400c3d:	5d                   	pop    %rbp
  400c3e:	bf 00 1a 60 00       	mov    $0x601a00,%edi
  400c43:	ff e0                	jmpq   *%rax
  400c45:	0f 1f 00             	nopl   (%rax)
  400c48:	5d                   	pop    %rbp
  400c49:	c3                   	retq   
  400c4a:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)

0000000000400c50 <__do_global_dtors_aux>:
  400c50:	80 3d 91 10 20 00 00 	cmpb   $0x0,0x201091(%rip)        # 601ce8 <completed.7585>
  400c57:	75 11                	jne    400c6a <__do_global_dtors_aux+0x1a>
  400c59:	55                   	push   %rbp
  400c5a:	48 89 e5             	mov    %rsp,%rbp
  400c5d:	e8 6e ff ff ff       	callq  400bd0 <deregister_tm_clones>
  400c62:	5d                   	pop    %rbp
  400c63:	c6 05 7e 10 20 00 01 	movb   $0x1,0x20107e(%rip)        # 601ce8 <completed.7585>
  400c6a:	f3 c3                	repz retq 
  400c6c:	0f 1f 40 00          	nopl   0x0(%rax)

0000000000400c70 <frame_dummy>:
  400c70:	bf 60 17 60 00       	mov    $0x601760,%edi
  400c75:	48 83 3f 00          	cmpq   $0x0,(%rdi)
  400c79:	75 05                	jne    400c80 <frame_dummy+0x10>
  400c7b:	eb 93                	jmp    400c10 <register_tm_clones>
  400c7d:	0f 1f 00             	nopl   (%rax)
  400c80:	b8 00 00 00 00       	mov    $0x0,%eax
  400c85:	48 85 c0             	test   %rax,%rax
  400c88:	74 f1                	je     400c7b <frame_dummy+0xb>
  400c8a:	55                   	push   %rbp
  400c8b:	48 89 e5             	mov    %rsp,%rbp
  400c8e:	ff d0                	callq  *%rax
  400c90:	5d                   	pop    %rbp
  400c91:	e9 7a ff ff ff       	jmpq   400c10 <register_tm_clones>
  400c96:	66 2e 0f 1f 84 00 00 	nopw   %cs:0x0(%rax,%rax,1)
  400c9d:	00 00 00 

0000000000400ca0 <main>:
  400ca0:	41 57                	push   %r15
  400ca2:	41 56                	push   %r14
  400ca4:	41 55                	push   %r13
  400ca6:	41 54                	push   %r12
  400ca8:	53                   	push   %rbx
  400ca9:	48 81 ec 90 00 00 00 	sub    $0x90,%rsp
  400cb0:	0f 57 c0             	xorps  %xmm0,%xmm0
  400cb3:	0f 29 44 24 70       	movaps %xmm0,0x70(%rsp)
  400cb8:	48 c7 84 24 80 00 00 	movq   $0x0,0x80(%rsp)
  400cbf:	00 00 00 00 00 
  400cc4:	0f 28 05 15 07 00 00 	movaps 0x715(%rip),%xmm0        # 4013e0 <_IO_stdin_used+0x10>
  400ccb:	0f 29 44 24 50       	movaps %xmm0,0x50(%rsp)
  400cd0:	0f 28 05 19 07 00 00 	movaps 0x719(%rip),%xmm0        # 4013f0 <_IO_stdin_used+0x20>
  400cd7:	0f 29 44 24 60       	movaps %xmm0,0x60(%rsp)
  400cdc:	48 8d 7c 24 70       	lea    0x70(%rsp),%rdi
  400ce1:	48 8d 74 24 50       	lea    0x50(%rsp),%rsi
  400ce6:	e8 05 05 00 00       	callq  4011f0 <_ZNSt6vectorISt5tupleIJllllEESaIS1_EE19_M_emplace_back_auxIJS1_EEEvDpOT_>
  400ceb:	48 8b 5c 24 78       	mov    0x78(%rsp),%rbx
  400cf0:	0f 28 05 09 07 00 00 	movaps 0x709(%rip),%xmm0        # 401400 <_IO_stdin_used+0x30>
  400cf7:	48 3b 9c 24 80 00 00 	cmp    0x80(%rsp),%rbx
  400cfe:	00 
  400cff:	0f 29 44 24 30       	movaps %xmm0,0x30(%rsp)
  400d04:	0f 28 0d 05 07 00 00 	movaps 0x705(%rip),%xmm1        # 401410 <_IO_stdin_used+0x40>
  400d0b:	0f 29 4c 24 40       	movaps %xmm1,0x40(%rsp)
  400d10:	74 12                	je     400d24 <main+0x84>
  400d12:	0f 11 03             	movups %xmm0,(%rbx)
  400d15:	0f 11 4b 10          	movups %xmm1,0x10(%rbx)
  400d19:	48 83 c3 20          	add    $0x20,%rbx
  400d1d:	48 89 5c 24 78       	mov    %rbx,0x78(%rsp)
  400d22:	eb 14                	jmp    400d38 <main+0x98>
  400d24:	48 8d 7c 24 70       	lea    0x70(%rsp),%rdi
  400d29:	48 8d 74 24 30       	lea    0x30(%rsp),%rsi
  400d2e:	e8 bd 04 00 00       	callq  4011f0 <_ZNSt6vectorISt5tupleIJllllEESaIS1_EE19_M_emplace_back_auxIJS1_EEEvDpOT_>
  400d33:	48 8b 5c 24 78       	mov    0x78(%rsp),%rbx
  400d38:	48 c7 44 24 08 30 14 	movq   $0x401430,0x8(%rsp)
  400d3f:	40 00 
  400d41:	0f 57 c0             	xorps  %xmm0,%xmm0
  400d44:	0f 11 44 24 20       	movups %xmm0,0x20(%rsp)
  400d49:	0f 11 44 24 10       	movups %xmm0,0x10(%rsp)
  400d4e:	4c 8b 74 24 70       	mov    0x70(%rsp),%r14
  400d53:	49 89 dd             	mov    %rbx,%r13
  400d56:	4d 29 f5             	sub    %r14,%r13
  400d59:	4d 89 ec             	mov    %r13,%r12
  400d5c:	49 c1 fc 05          	sar    $0x5,%r12
  400d60:	74 7f                	je     400de1 <main+0x141>
  400d62:	4c 89 e0             	mov    %r12,%rax
  400d65:	48 c1 e8 3b          	shr    $0x3b,%rax
  400d69:	0f 85 41 04 00 00    	jne    4011b0 <main+0x510>
  400d6f:	4c 89 ef             	mov    %r13,%rdi
  400d72:	e8 c9 fd ff ff       	callq  400b40 <_Znwm@plt>
  400d77:	49 89 c7             	mov    %rax,%r15
  400d7a:	49 39 de             	cmp    %rbx,%r14
  400d7d:	0f 84 91 01 00 00    	je     400f14 <main+0x274>
  400d83:	49 83 c5 e0          	add    $0xffffffffffffffe0,%r13
  400d87:	44 89 e8             	mov    %r13d,%eax
  400d8a:	c1 e8 05             	shr    $0x5,%eax
  400d8d:	ff c0                	inc    %eax
  400d8f:	a8 07                	test   $0x7,%al
  400d91:	0f 84 9d 00 00 00    	je     400e34 <main+0x194>
  400d97:	44 89 e9             	mov    %r13d,%ecx
  400d9a:	c1 e9 05             	shr    $0x5,%ecx
  400d9d:	ff c1                	inc    %ecx
  400d9f:	83 e1 07             	and    $0x7,%ecx
  400da2:	48 89 ca             	mov    %rcx,%rdx
  400da5:	48 c1 e2 05          	shl    $0x5,%rdx
  400da9:	49 8d 04 17          	lea    (%r15,%rdx,1),%rax
  400dad:	48 f7 d9             	neg    %rcx
  400db0:	4c 89 fe             	mov    %r15,%rsi
  400db3:	4c 89 f7             	mov    %r14,%rdi
  400db6:	66 2e 0f 1f 84 00 00 	nopw   %cs:0x0(%rax,%rax,1)
  400dbd:	00 00 00 
  400dc0:	0f 10 07             	movups (%rdi),%xmm0
  400dc3:	0f 10 4f 10          	movups 0x10(%rdi),%xmm1
  400dc7:	0f 11 4e 10          	movups %xmm1,0x10(%rsi)
  400dcb:	0f 11 06             	movups %xmm0,(%rsi)
  400dce:	48 83 c7 20          	add    $0x20,%rdi
  400dd2:	48 83 c6 20          	add    $0x20,%rsi
  400dd6:	48 ff c1             	inc    %rcx
  400dd9:	75 e5                	jne    400dc0 <main+0x120>
  400ddb:	4a 8d 0c 32          	lea    (%rdx,%r14,1),%rcx
  400ddf:	eb 59                	jmp    400e3a <main+0x19a>
  400de1:	48 8d 44 24 18       	lea    0x18(%rsp),%rax
  400de6:	45 31 ff             	xor    %r15d,%r15d
  400de9:	b9 30 14 40 00       	mov    $0x401430,%ecx
  400dee:	4d 85 ed             	test   %r13,%r13
  400df1:	0f 8e 4d 01 00 00    	jle    400f44 <main+0x2a4>
  400df7:	49 8d 54 24 01       	lea    0x1(%r12),%rdx
  400dfc:	be 10 00 00 00       	mov    $0x10,%esi
  400e01:	45 31 ff             	xor    %r15d,%r15d
  400e04:	b9 30 14 40 00       	mov    $0x401430,%ecx
  400e09:	0f 1f 80 00 00 00 00 	nopl   0x0(%rax)
  400e10:	41 0f 10 04 36       	movups (%r14,%rsi,1),%xmm0
  400e15:	0f 11 06             	movups %xmm0,(%rsi)
  400e18:	41 0f 10 44 36 f0    	movups -0x10(%r14,%rsi,1),%xmm0
  400e1e:	0f 11 46 f0          	movups %xmm0,-0x10(%rsi)
  400e22:	48 ff ca             	dec    %rdx
  400e25:	48 83 c6 20          	add    $0x20,%rsi
  400e29:	48 83 fa 01          	cmp    $0x1,%rdx
  400e2d:	7f e1                	jg     400e10 <main+0x170>
  400e2f:	e9 10 01 00 00       	jmpq   400f44 <main+0x2a4>
  400e34:	4c 89 f8             	mov    %r15,%rax
  400e37:	4c 89 f1             	mov    %r14,%rcx
  400e3a:	49 81 fd e0 00 00 00 	cmp    $0xe0,%r13
  400e41:	0f 82 cd 00 00 00    	jb     400f14 <main+0x274>
  400e47:	66 0f 1f 84 00 00 00 	nopw   0x0(%rax,%rax,1)
  400e4e:	00 00 
  400e50:	0f 10 01             	movups (%rcx),%xmm0
  400e53:	0f 10 49 10          	movups 0x10(%rcx),%xmm1
  400e57:	0f 11 48 10          	movups %xmm1,0x10(%rax)
  400e5b:	0f 11 00             	movups %xmm0,(%rax)
  400e5e:	0f 10 41 20          	movups 0x20(%rcx),%xmm0
  400e62:	0f 10 49 30          	movups 0x30(%rcx),%xmm1
  400e66:	0f 11 48 30          	movups %xmm1,0x30(%rax)
  400e6a:	0f 11 40 20          	movups %xmm0,0x20(%rax)
  400e6e:	0f 10 41 40          	movups 0x40(%rcx),%xmm0
  400e72:	0f 10 49 50          	movups 0x50(%rcx),%xmm1
  400e76:	0f 11 48 50          	movups %xmm1,0x50(%rax)
  400e7a:	0f 11 40 40          	movups %xmm0,0x40(%rax)
  400e7e:	0f 10 41 60          	movups 0x60(%rcx),%xmm0
  400e82:	0f 10 49 70          	movups 0x70(%rcx),%xmm1
  400e86:	0f 11 48 70          	movups %xmm1,0x70(%rax)
  400e8a:	0f 11 40 60          	movups %xmm0,0x60(%rax)
  400e8e:	0f 10 81 80 00 00 00 	movups 0x80(%rcx),%xmm0
  400e95:	0f 10 89 90 00 00 00 	movups 0x90(%rcx),%xmm1
  400e9c:	0f 11 88 90 00 00 00 	movups %xmm1,0x90(%rax)
  400ea3:	0f 11 80 80 00 00 00 	movups %xmm0,0x80(%rax)
  400eaa:	0f 10 81 a0 00 00 00 	movups 0xa0(%rcx),%xmm0
  400eb1:	0f 10 89 b0 00 00 00 	movups 0xb0(%rcx),%xmm1
  400eb8:	0f 11 88 b0 00 00 00 	movups %xmm1,0xb0(%rax)
  400ebf:	0f 11 80 a0 00 00 00 	movups %xmm0,0xa0(%rax)
  400ec6:	0f 10 81 c0 00 00 00 	movups 0xc0(%rcx),%xmm0
  400ecd:	0f 10 89 d0 00 00 00 	movups 0xd0(%rcx),%xmm1
  400ed4:	0f 11 88 d0 00 00 00 	movups %xmm1,0xd0(%rax)
  400edb:	0f 11 80 c0 00 00 00 	movups %xmm0,0xc0(%rax)
  400ee2:	0f 10 81 e0 00 00 00 	movups 0xe0(%rcx),%xmm0
  400ee9:	0f 10 89 f0 00 00 00 	movups 0xf0(%rcx),%xmm1
  400ef0:	0f 11 88 f0 00 00 00 	movups %xmm1,0xf0(%rax)
  400ef7:	0f 11 80 e0 00 00 00 	movups %xmm0,0xe0(%rax)
  400efe:	48 81 c1 00 01 00 00 	add    $0x100,%rcx
  400f05:	48 05 00 01 00 00    	add    $0x100,%rax
  400f0b:	48 39 d9             	cmp    %rbx,%rcx
  400f0e:	0f 85 3c ff ff ff    	jne    400e50 <main+0x1b0>
  400f14:	48 8b 7c 24 10       	mov    0x10(%rsp),%rdi
  400f19:	48 85 ff             	test   %rdi,%rdi
  400f1c:	74 05                	je     400f23 <main+0x283>
  400f1e:	e8 7d fb ff ff       	callq  400aa0 <_ZdlPv@plt>
  400f23:	4c 89 7c 24 10       	mov    %r15,0x10(%rsp)
  400f28:	4c 89 e0             	mov    %r12,%rax
  400f2b:	48 c1 e0 05          	shl    $0x5,%rax
  400f2f:	4c 89 f9             	mov    %r15,%rcx
  400f32:	48 01 c1             	add    %rax,%rcx
  400f35:	48 89 4c 24 20       	mov    %rcx,0x20(%rsp)
  400f3a:	48 8d 44 24 18       	lea    0x18(%rsp),%rax
  400f3f:	48 8b 4c 24 08       	mov    0x8(%rsp),%rcx
  400f44:	49 c1 e4 05          	shl    $0x5,%r12
  400f48:	4d 01 fc             	add    %r15,%r12
  400f4b:	4c 89 20             	mov    %r12,(%rax)
  400f4e:	48 8b 41 08          	mov    0x8(%rcx),%rax
  400f52:	48 8d 7c 24 08       	lea    0x8(%rsp),%rdi
  400f57:	ff d0                	callq  *%rax
  400f59:	0f 1f 80 00 00 00 00 	nopl   0x0(%rax)
  400f60:	48 8b 44 24 28       	mov    0x28(%rsp),%rax
  400f65:	48 3b 44 24 18       	cmp    0x18(%rsp),%rax
  400f6a:	0f 84 cc 01 00 00    	je     40113c <main+0x49c>
  400f70:	4c 8b 60 10          	mov    0x10(%rax),%r12
  400f74:	4c 8b 78 18          	mov    0x18(%rax),%r15
  400f78:	48 83 c0 20          	add    $0x20,%rax
  400f7c:	48 89 44 24 28       	mov    %rax,0x28(%rsp)
  400f81:	bf 00 1a 60 00       	mov    $0x601a00,%edi
  400f86:	be ab 14 40 00       	mov    $0x4014ab,%esi
  400f8b:	ba 16 00 00 00       	mov    $0x16,%edx
  400f90:	e8 6b fb ff ff       	callq  400b00 <_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@plt>
  400f95:	bf 00 1a 60 00       	mov    $0x601a00,%edi
  400f9a:	be c2 14 40 00       	mov    $0x4014c2,%esi
  400f9f:	ba 02 00 00 00       	mov    $0x2,%edx
  400fa4:	e8 57 fb ff ff       	callq  400b00 <_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@plt>
  400fa9:	bf 00 1a 60 00       	mov    $0x601a00,%edi
  400fae:	4c 89 fe             	mov    %r15,%rsi
  400fb1:	e8 da fa ff ff       	callq  400a90 <_ZNSo9_M_insertIlEERSoT_@plt>
  400fb6:	49 89 c5             	mov    %rax,%r13
  400fb9:	49 8b 45 00          	mov    0x0(%r13),%rax
  400fbd:	48 8b 40 e8          	mov    -0x18(%rax),%rax
  400fc1:	49 8b 9c 05 f0 00 00 	mov    0xf0(%r13,%rax,1),%rbx
  400fc8:	00 
  400fc9:	48 85 db             	test   %rbx,%rbx
  400fcc:	0f 84 4f 01 00 00    	je     401121 <main+0x481>
  400fd2:	80 7b 38 00          	cmpb   $0x0,0x38(%rbx)
  400fd6:	74 08                	je     400fe0 <main+0x340>
  400fd8:	8a 43 43             	mov    0x43(%rbx),%al
  400fdb:	eb 1c                	jmp    400ff9 <main+0x359>
  400fdd:	0f 1f 00             	nopl   (%rax)
  400fe0:	48 89 df             	mov    %rbx,%rdi
  400fe3:	e8 c8 fa ff ff       	callq  400ab0 <_ZNKSt5ctypeIcE13_M_widen_initEv@plt>
  400fe8:	48 8b 03             	mov    (%rbx),%rax
  400feb:	48 8b 40 30          	mov    0x30(%rax),%rax
  400fef:	be 0a 00 00 00       	mov    $0xa,%esi
  400ff4:	48 89 df             	mov    %rbx,%rdi
  400ff7:	ff d0                	callq  *%rax
  400ff9:	0f be f0             	movsbl %al,%esi
  400ffc:	4c 89 ef             	mov    %r13,%rdi
  400fff:	e8 5c fb ff ff       	callq  400b60 <_ZNSo3putEc@plt>
  401004:	48 89 c7             	mov    %rax,%rdi
  401007:	e8 04 fb ff ff       	callq  400b10 <_ZNSo5flushEv@plt>
  40100c:	bf 00 1a 60 00       	mov    $0x601a00,%edi
  401011:	be d4 14 40 00       	mov    $0x4014d4,%esi
  401016:	ba 10 00 00 00       	mov    $0x10,%edx
  40101b:	e8 e0 fa ff ff       	callq  400b00 <_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@plt>
  401020:	bf 00 1a 60 00       	mov    $0x601a00,%edi
  401025:	be c2 14 40 00       	mov    $0x4014c2,%esi
  40102a:	ba 02 00 00 00       	mov    $0x2,%edx
  40102f:	e8 cc fa ff ff       	callq  400b00 <_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@plt>
  401034:	bf 00 1a 60 00       	mov    $0x601a00,%edi
  401039:	4c 89 e6             	mov    %r12,%rsi
  40103c:	e8 4f fa ff ff       	callq  400a90 <_ZNSo9_M_insertIlEERSoT_@plt>
  401041:	49 89 c5             	mov    %rax,%r13
  401044:	49 8b 45 00          	mov    0x0(%r13),%rax
  401048:	48 8b 40 e8          	mov    -0x18(%rax),%rax
  40104c:	49 8b 9c 05 f0 00 00 	mov    0xf0(%r13,%rax,1),%rbx
  401053:	00 
  401054:	48 85 db             	test   %rbx,%rbx
  401057:	0f 84 ce 00 00 00    	je     40112b <main+0x48b>
  40105d:	80 7b 38 00          	cmpb   $0x0,0x38(%rbx)
  401061:	74 0d                	je     401070 <main+0x3d0>
  401063:	8a 43 43             	mov    0x43(%rbx),%al
  401066:	eb 21                	jmp    401089 <main+0x3e9>
  401068:	0f 1f 84 00 00 00 00 	nopl   0x0(%rax,%rax,1)
  40106f:	00 
  401070:	48 89 df             	mov    %rbx,%rdi
  401073:	e8 38 fa ff ff       	callq  400ab0 <_ZNKSt5ctypeIcE13_M_widen_initEv@plt>
  401078:	48 8b 03             	mov    (%rbx),%rax
  40107b:	48 8b 40 30          	mov    0x30(%rax),%rax
  40107f:	be 0a 00 00 00       	mov    $0xa,%esi
  401084:	48 89 df             	mov    %rbx,%rdi
  401087:	ff d0                	callq  *%rax
  401089:	0f be f0             	movsbl %al,%esi
  40108c:	4c 89 ef             	mov    %r13,%rdi
  40108f:	e8 cc fa ff ff       	callq  400b60 <_ZNSo3putEc@plt>
  401094:	48 89 c7             	mov    %rax,%rdi
  401097:	e8 74 fa ff ff       	callq  400b10 <_ZNSo5flushEv@plt>
  40109c:	bf 80 1b 60 00       	mov    $0x601b80,%edi
  4010a1:	4c 89 e6             	mov    %r12,%rsi
  4010a4:	e8 e7 f9 ff ff       	callq  400a90 <_ZNSo9_M_insertIlEERSoT_@plt>
  4010a9:	48 89 c3             	mov    %rax,%rbx
  4010ac:	be a8 14 40 00       	mov    $0x4014a8,%esi
  4010b1:	ba 02 00 00 00       	mov    $0x2,%edx
  4010b6:	48 89 df             	mov    %rbx,%rdi
  4010b9:	e8 42 fa ff ff       	callq  400b00 <_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@plt>
  4010be:	48 89 df             	mov    %rbx,%rdi
  4010c1:	4c 89 fe             	mov    %r15,%rsi
  4010c4:	e8 c7 f9 ff ff       	callq  400a90 <_ZNSo9_M_insertIlEERSoT_@plt>
  4010c9:	49 89 c7             	mov    %rax,%r15
  4010cc:	49 8b 07             	mov    (%r15),%rax
  4010cf:	48 8b 40 e8          	mov    -0x18(%rax),%rax
  4010d3:	49 8b 9c 07 f0 00 00 	mov    0xf0(%r15,%rax,1),%rbx
  4010da:	00 
  4010db:	48 85 db             	test   %rbx,%rbx
  4010de:	74 55                	je     401135 <main+0x495>
  4010e0:	80 7b 38 00          	cmpb   $0x0,0x38(%rbx)
  4010e4:	74 0a                	je     4010f0 <main+0x450>
  4010e6:	8a 43 43             	mov    0x43(%rbx),%al
  4010e9:	eb 1e                	jmp    401109 <main+0x469>
  4010eb:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
  4010f0:	48 89 df             	mov    %rbx,%rdi
  4010f3:	e8 b8 f9 ff ff       	callq  400ab0 <_ZNKSt5ctypeIcE13_M_widen_initEv@plt>
  4010f8:	48 8b 03             	mov    (%rbx),%rax
  4010fb:	48 8b 40 30          	mov    0x30(%rax),%rax
  4010ff:	be 0a 00 00 00       	mov    $0xa,%esi
  401104:	48 89 df             	mov    %rbx,%rdi
  401107:	ff d0                	callq  *%rax
  401109:	0f be f0             	movsbl %al,%esi
  40110c:	4c 89 ff             	mov    %r15,%rdi
  40110f:	e8 4c fa ff ff       	callq  400b60 <_ZNSo3putEc@plt>
  401114:	48 89 c7             	mov    %rax,%rdi
  401117:	e8 f4 f9 ff ff       	callq  400b10 <_ZNSo5flushEv@plt>
  40111c:	e9 3f fe ff ff       	jmpq   400f60 <main+0x2c0>
  401121:	e8 5a f9 ff ff       	callq  400a80 <_ZSt16__throw_bad_castv@plt>
  401126:	e9 a7 fe ff ff       	jmpq   400fd2 <main+0x332>
  40112b:	e8 50 f9 ff ff       	callq  400a80 <_ZSt16__throw_bad_castv@plt>
  401130:	e9 28 ff ff ff       	jmpq   40105d <main+0x3bd>
  401135:	e8 46 f9 ff ff       	callq  400a80 <_ZSt16__throw_bad_castv@plt>
  40113a:	eb a4                	jmp    4010e0 <main+0x440>
  40113c:	48 c7 44 24 08 30 14 	movq   $0x401430,0x8(%rsp)
  401143:	40 00 
  401145:	48 8b 7c 24 10       	mov    0x10(%rsp),%rdi
  40114a:	48 85 ff             	test   %rdi,%rdi
  40114d:	74 05                	je     401154 <main+0x4b4>
  40114f:	e8 4c f9 ff ff       	callq  400aa0 <_ZdlPv@plt>
  401154:	4d 85 f6             	test   %r14,%r14
  401157:	74 08                	je     401161 <main+0x4c1>
  401159:	4c 89 f7             	mov    %r14,%rdi
  40115c:	e8 3f f9 ff ff       	callq  400aa0 <_ZdlPv@plt>
  401161:	31 c0                	xor    %eax,%eax
  401163:	48 81 c4 90 00 00 00 	add    $0x90,%rsp
  40116a:	5b                   	pop    %rbx
  40116b:	41 5c                	pop    %r12
  40116d:	41 5d                	pop    %r13
  40116f:	41 5e                	pop    %r14
  401171:	41 5f                	pop    %r15
  401173:	c3                   	retq   
  401174:	48 89 c3             	mov    %rax,%rbx
  401177:	48 c7 44 24 08 30 14 	movq   $0x401430,0x8(%rsp)
  40117e:	40 00 
  401180:	48 8b 7c 24 10       	mov    0x10(%rsp),%rdi
  401185:	48 85 ff             	test   %rdi,%rdi
  401188:	74 05                	je     40118f <main+0x4ef>
  40118a:	e8 11 f9 ff ff       	callq  400aa0 <_ZdlPv@plt>
  40118f:	4d 85 f6             	test   %r14,%r14
  401192:	74 08                	je     40119c <main+0x4fc>
  401194:	4c 89 f7             	mov    %r14,%rdi
  401197:	e8 04 f9 ff ff       	callq  400aa0 <_ZdlPv@plt>
  40119c:	48 89 df             	mov    %rbx,%rdi
  40119f:	e8 ac f9 ff ff       	callq  400b50 <_Unwind_Resume@plt>
  4011a4:	eb ce                	jmp    401174 <main+0x4d4>
  4011a6:	48 89 c3             	mov    %rax,%rbx
  4011a9:	4c 8b 74 24 70       	mov    0x70(%rsp),%r14
  4011ae:	eb df                	jmp    40118f <main+0x4ef>
  4011b0:	e8 6b f9 ff ff       	callq  400b20 <_ZSt17__throw_bad_allocv@plt>
  4011b5:	e9 b5 fb ff ff       	jmpq   400d6f <main+0xcf>
  4011ba:	eb b8                	jmp    401174 <main+0x4d4>
  4011bc:	0f 1f 40 00          	nopl   0x0(%rax)

00000000004011c0 <_ZN24CollectionSourceOperatorISt5tupleIJllllEEE9printNameEv>:
  4011c0:	bf 80 1b 60 00       	mov    $0x601b80,%edi
  4011c5:	be c5 14 40 00       	mov    $0x4014c5,%esi
  4011ca:	ba 0e 00 00 00       	mov    $0xe,%edx
  4011cf:	e9 2c f9 ff ff       	jmpq   400b00 <_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@plt>
  4011d4:	66 2e 0f 1f 84 00 00 	nopw   %cs:0x0(%rax,%rax,1)
  4011db:	00 00 00 
  4011de:	66 90                	xchg   %ax,%ax

00000000004011e0 <_ZN24CollectionSourceOperatorISt5tupleIJllllEEE4openEv>:
  4011e0:	48 8b 47 08          	mov    0x8(%rdi),%rax
  4011e4:	48 89 47 20          	mov    %rax,0x20(%rdi)
  4011e8:	c3                   	retq   
  4011e9:	0f 1f 80 00 00 00 00 	nopl   0x0(%rax)

00000000004011f0 <_ZNSt6vectorISt5tupleIJllllEESaIS1_EE19_M_emplace_back_auxIJS1_EEEvDpOT_>:
  4011f0:	41 57                	push   %r15
  4011f2:	41 56                	push   %r14
  4011f4:	41 55                	push   %r13
  4011f6:	41 54                	push   %r12
  4011f8:	53                   	push   %rbx
  4011f9:	49 89 f4             	mov    %rsi,%r12
  4011fc:	49 89 fe             	mov    %rdi,%r14
  4011ff:	49 8b 46 08          	mov    0x8(%r14),%rax
  401203:	49 2b 06             	sub    (%r14),%rax
  401206:	48 c1 f8 05          	sar    $0x5,%rax
  40120a:	b9 01 00 00 00       	mov    $0x1,%ecx
  40120f:	48 0f 45 c8          	cmovne %rax,%rcx
  401213:	4c 8d 3c 01          	lea    (%rcx,%rax,1),%r15
  401217:	4c 89 fa             	mov    %r15,%rdx
  40121a:	48 c1 ea 3b          	shr    $0x3b,%rdx
  40121e:	48 ba ff ff ff ff ff 	movabs $0x7ffffffffffffff,%rdx
  401225:	ff ff 07 
  401228:	4c 0f 45 fa          	cmovne %rdx,%r15
  40122c:	48 01 c1             	add    %rax,%rcx
  40122f:	4c 0f 42 fa          	cmovb  %rdx,%r15
  401233:	4c 89 f8             	mov    %r15,%rax
  401236:	48 c1 e8 3b          	shr    $0x3b,%rax
  40123a:	0f 85 0a 01 00 00    	jne    40134a <_ZNSt6vectorISt5tupleIJllllEESaIS1_EE19_M_emplace_back_auxIJS1_EEEvDpOT_+0x15a>
  401240:	49 c1 e7 05          	shl    $0x5,%r15
  401244:	4c 89 ff             	mov    %r15,%rdi
  401247:	e8 f4 f8 ff ff       	callq  400b40 <_Znwm@plt>
  40124c:	49 89 c5             	mov    %rax,%r13
  40124f:	49 8b 3e             	mov    (%r14),%rdi
  401252:	49 8b 46 08          	mov    0x8(%r14),%rax
  401256:	48 89 c1             	mov    %rax,%rcx
  401259:	48 29 f9             	sub    %rdi,%rcx
  40125c:	41 0f 10 04 24       	movups (%r12),%xmm0
  401261:	41 0f 11 44 0d 00    	movups %xmm0,0x0(%r13,%rcx,1)
  401267:	41 0f 10 44 24 10    	movups 0x10(%r12),%xmm0
  40126d:	41 0f 11 44 0d 10    	movups %xmm0,0x10(%r13,%rcx,1)
  401273:	4c 89 eb             	mov    %r13,%rbx
  401276:	0f 84 a8 00 00 00    	je     401324 <_ZNSt6vectorISt5tupleIJllllEESaIS1_EE19_M_emplace_back_auxIJS1_EEEvDpOT_+0x134>
  40127c:	48 8d 48 e0          	lea    -0x20(%rax),%rcx
  401280:	48 29 f9             	sub    %rdi,%rcx
  401283:	49 89 c8             	mov    %rcx,%r8
  401286:	49 c1 e8 05          	shr    $0x5,%r8
  40128a:	48 0f ba e1 05       	bt     $0x5,%rcx
  40128f:	72 29                	jb     4012ba <_ZNSt6vectorISt5tupleIJllllEESaIS1_EE19_M_emplace_back_auxIJS1_EEEvDpOT_+0xca>
  401291:	48 8b 17             	mov    (%rdi),%rdx
  401294:	49 89 55 00          	mov    %rdx,0x0(%r13)
  401298:	48 8b 57 08          	mov    0x8(%rdi),%rdx
  40129c:	49 89 55 08          	mov    %rdx,0x8(%r13)
  4012a0:	48 8b 57 10          	mov    0x10(%rdi),%rdx
  4012a4:	49 89 55 10          	mov    %rdx,0x10(%r13)
  4012a8:	48 8b 57 18          	mov    0x18(%rdi),%rdx
  4012ac:	49 89 55 18          	mov    %rdx,0x18(%r13)
  4012b0:	48 8d 77 20          	lea    0x20(%rdi),%rsi
  4012b4:	49 8d 55 20          	lea    0x20(%r13),%rdx
  4012b8:	eb 06                	jmp    4012c0 <_ZNSt6vectorISt5tupleIJllllEESaIS1_EE19_M_emplace_back_auxIJS1_EEEvDpOT_+0xd0>
  4012ba:	4c 89 ea             	mov    %r13,%rdx
  4012bd:	48 89 fe             	mov    %rdi,%rsi
  4012c0:	4d 85 c0             	test   %r8,%r8
  4012c3:	74 56                	je     40131b <_ZNSt6vectorISt5tupleIJllllEESaIS1_EE19_M_emplace_back_auxIJS1_EEEvDpOT_+0x12b>
  4012c5:	66 66 2e 0f 1f 84 00 	data16 nopw %cs:0x0(%rax,%rax,1)
  4012cc:	00 00 00 00 
  4012d0:	48 8b 1e             	mov    (%rsi),%rbx
  4012d3:	48 89 1a             	mov    %rbx,(%rdx)
  4012d6:	48 8b 5e 08          	mov    0x8(%rsi),%rbx
  4012da:	48 89 5a 08          	mov    %rbx,0x8(%rdx)
  4012de:	48 8b 5e 10          	mov    0x10(%rsi),%rbx
  4012e2:	48 89 5a 10          	mov    %rbx,0x10(%rdx)
  4012e6:	48 8b 5e 18          	mov    0x18(%rsi),%rbx
  4012ea:	48 89 5a 18          	mov    %rbx,0x18(%rdx)
  4012ee:	48 8b 5e 20          	mov    0x20(%rsi),%rbx
  4012f2:	48 89 5a 20          	mov    %rbx,0x20(%rdx)
  4012f6:	48 8b 5e 28          	mov    0x28(%rsi),%rbx
  4012fa:	48 89 5a 28          	mov    %rbx,0x28(%rdx)
  4012fe:	48 8b 5e 30          	mov    0x30(%rsi),%rbx
  401302:	48 89 5a 30          	mov    %rbx,0x30(%rdx)
  401306:	48 8b 5e 38          	mov    0x38(%rsi),%rbx
  40130a:	48 89 5a 38          	mov    %rbx,0x38(%rdx)
  40130e:	48 83 c6 40          	add    $0x40,%rsi
  401312:	48 83 c2 40          	add    $0x40,%rdx
  401316:	48 39 c6             	cmp    %rax,%rsi
  401319:	75 b5                	jne    4012d0 <_ZNSt6vectorISt5tupleIJllllEESaIS1_EE19_M_emplace_back_auxIJS1_EEEvDpOT_+0xe0>
  40131b:	48 83 e1 e0          	and    $0xffffffffffffffe0,%rcx
  40131f:	4a 8d 5c 29 20       	lea    0x20(%rcx,%r13,1),%rbx
  401324:	48 83 c3 20          	add    $0x20,%rbx
  401328:	48 85 ff             	test   %rdi,%rdi
  40132b:	74 05                	je     401332 <_ZNSt6vectorISt5tupleIJllllEESaIS1_EE19_M_emplace_back_auxIJS1_EEEvDpOT_+0x142>
  40132d:	e8 6e f7 ff ff       	callq  400aa0 <_ZdlPv@plt>
  401332:	4d 89 2e             	mov    %r13,(%r14)
  401335:	49 89 5e 08          	mov    %rbx,0x8(%r14)
  401339:	4d 01 fd             	add    %r15,%r13
  40133c:	4d 89 6e 10          	mov    %r13,0x10(%r14)
  401340:	5b                   	pop    %rbx
  401341:	41 5c                	pop    %r12
  401343:	41 5d                	pop    %r13
  401345:	41 5e                	pop    %r14
  401347:	41 5f                	pop    %r15
  401349:	c3                   	retq   
  40134a:	e8 d1 f7 ff ff       	callq  400b20 <_ZSt17__throw_bad_allocv@plt>
  40134f:	90                   	nop

0000000000401350 <__libc_csu_init>:
  401350:	41 57                	push   %r15
  401352:	41 56                	push   %r14
  401354:	41 89 ff             	mov    %edi,%r15d
  401357:	41 55                	push   %r13
  401359:	41 54                	push   %r12
  40135b:	4c 8d 25 e6 03 20 00 	lea    0x2003e6(%rip),%r12        # 601748 <__frame_dummy_init_array_entry>
  401362:	55                   	push   %rbp
  401363:	48 8d 2d ee 03 20 00 	lea    0x2003ee(%rip),%rbp        # 601758 <__init_array_end>
  40136a:	53                   	push   %rbx
  40136b:	49 89 f6             	mov    %rsi,%r14
  40136e:	49 89 d5             	mov    %rdx,%r13
  401371:	4c 29 e5             	sub    %r12,%rbp
  401374:	48 83 ec 08          	sub    $0x8,%rsp
  401378:	48 c1 fd 03          	sar    $0x3,%rbp
  40137c:	e8 cf f6 ff ff       	callq  400a50 <_init>
  401381:	48 85 ed             	test   %rbp,%rbp
  401384:	74 20                	je     4013a6 <__libc_csu_init+0x56>
  401386:	31 db                	xor    %ebx,%ebx
  401388:	0f 1f 84 00 00 00 00 	nopl   0x0(%rax,%rax,1)
  40138f:	00 
  401390:	4c 89 ea             	mov    %r13,%rdx
  401393:	4c 89 f6             	mov    %r14,%rsi
  401396:	44 89 ff             	mov    %r15d,%edi
  401399:	41 ff 14 dc          	callq  *(%r12,%rbx,8)
  40139d:	48 83 c3 01          	add    $0x1,%rbx
  4013a1:	48 39 eb             	cmp    %rbp,%rbx
  4013a4:	75 ea                	jne    401390 <__libc_csu_init+0x40>
  4013a6:	48 83 c4 08          	add    $0x8,%rsp
  4013aa:	5b                   	pop    %rbx
  4013ab:	5d                   	pop    %rbp
  4013ac:	41 5c                	pop    %r12
  4013ae:	41 5d                	pop    %r13
  4013b0:	41 5e                	pop    %r14
  4013b2:	41 5f                	pop    %r15
  4013b4:	c3                   	retq   
  4013b5:	90                   	nop
  4013b6:	66 2e 0f 1f 84 00 00 	nopw   %cs:0x0(%rax,%rax,1)
  4013bd:	00 00 00 

00000000004013c0 <__libc_csu_fini>:
  4013c0:	f3 c3                	repz retq 

Disassembly of section .fini:

00000000004013c4 <_fini>:
  4013c4:	48 83 ec 08          	sub    $0x8,%rsp
  4013c8:	48 83 c4 08          	add    $0x8,%rsp
  4013cc:	c3                   	retq   

Disassembly of section .rodata:

00000000004013d0 <_IO_stdin_used>:
  4013d0:	01 00                	add    %eax,(%rax)
  4013d2:	02 00                	add    (%rax),%al
	...
  4013e0:	04 00                	add    $0x0,%al
  4013e2:	00 00                	add    %al,(%rax)
  4013e4:	00 00                	add    %al,(%rax)
  4013e6:	00 00                	add    %al,(%rax)
  4013e8:	03 00                	add    (%rax),%eax
  4013ea:	00 00                	add    %al,(%rax)
  4013ec:	00 00                	add    %al,(%rax)
  4013ee:	00 00                	add    %al,(%rax)
  4013f0:	02 00                	add    (%rax),%al
  4013f2:	00 00                	add    %al,(%rax)
  4013f4:	00 00                	add    %al,(%rax)
  4013f6:	00 00                	add    %al,(%rax)
  4013f8:	01 00                	add    %eax,(%rax)
  4013fa:	00 00                	add    %al,(%rax)
  4013fc:	00 00                	add    %al,(%rax)
  4013fe:	00 00                	add    %al,(%rax)
  401400:	08 00                	or     %al,(%rax)
  401402:	00 00                	add    %al,(%rax)
  401404:	00 00                	add    %al,(%rax)
  401406:	00 00                	add    %al,(%rax)
  401408:	07                   	(bad)  
  401409:	00 00                	add    %al,(%rax)
  40140b:	00 00                	add    %al,(%rax)
  40140d:	00 00                	add    %al,(%rax)
  40140f:	00 06                	add    %al,(%rsi)
  401411:	00 00                	add    %al,(%rax)
  401413:	00 00                	add    %al,(%rax)
  401415:	00 00                	add    %al,(%rax)
  401417:	00 05 00 00 00 00    	add    %al,0x0(%rip)        # 40141d <_IO_stdin_used+0x4d>
  40141d:	00 00                	add    %al,(%rax)
	...

0000000000401420 <_ZTV24CollectionSourceOperatorISt5tupleIJllllEEE>:
	...
  401428:	90                   	nop
  401429:	14 40                	adc    $0x40,%al
  40142b:	00 00                	add    %al,(%rax)
  40142d:	00 00                	add    %al,(%rax)
  40142f:	00 c0                	add    %al,%al
  401431:	11 40 00             	adc    %eax,0x0(%rax)
  401434:	00 00                	add    %al,(%rax)
  401436:	00 00                	add    %al,(%rax)
  401438:	e0 11                	loopne 40144b <_ZTS24CollectionSourceOperatorISt5tupleIJllllEEE+0xb>
  40143a:	40 00 00             	add    %al,(%rax)
  40143d:	00 00                	add    %al,(%rax)
	...

0000000000401440 <_ZTS24CollectionSourceOperatorISt5tupleIJllllEEE>:
  401440:	32 34 43             	xor    (%rbx,%rax,2),%dh
  401443:	6f                   	outsl  %ds:(%rsi),(%dx)
  401444:	6c                   	insb   (%dx),%es:(%rdi)
  401445:	6c                   	insb   (%dx),%es:(%rdi)
  401446:	65 63 74 69 6f       	movslq %gs:0x6f(%rcx,%rbp,2),%esi
  40144b:	6e                   	outsb  %ds:(%rsi),(%dx)
  40144c:	53                   	push   %rbx
  40144d:	6f                   	outsl  %ds:(%rsi),(%dx)
  40144e:	75 72                	jne    4014c2 <_ZTI24CollectionSourceOperatorISt5tupleIJllllEEE+0x32>
  401450:	63 65 4f             	movslq 0x4f(%rbp),%esp
  401453:	70 65                	jo     4014ba <_ZTI24CollectionSourceOperatorISt5tupleIJllllEEE+0x2a>
  401455:	72 61                	jb     4014b8 <_ZTI24CollectionSourceOperatorISt5tupleIJllllEEE+0x28>
  401457:	74 6f                	je     4014c8 <_ZTI24CollectionSourceOperatorISt5tupleIJllllEEE+0x38>
  401459:	72 49                	jb     4014a4 <_ZTI24CollectionSourceOperatorISt5tupleIJllllEEE+0x14>
  40145b:	53                   	push   %rbx
  40145c:	74 35                	je     401493 <_ZTI24CollectionSourceOperatorISt5tupleIJllllEEE+0x3>
  40145e:	74 75                	je     4014d5 <_ZTI24CollectionSourceOperatorISt5tupleIJllllEEE+0x45>
  401460:	70 6c                	jo     4014ce <_ZTI24CollectionSourceOperatorISt5tupleIJllllEEE+0x3e>
  401462:	65 49                	gs rex.WB
  401464:	4a 6c                	rex.WX insb (%dx),%es:(%rdi)
  401466:	6c                   	insb   (%dx),%es:(%rdi)
  401467:	6c                   	insb   (%dx),%es:(%rdi)
  401468:	6c                   	insb   (%dx),%es:(%rdi)
  401469:	45                   	rex.RB
  40146a:	45                   	rex.RB
  40146b:	45 00 38             	add    %r15b,(%r8)

000000000040146d <_ZTS8Operator>:
  40146d:	38 4f 70             	cmp    %cl,0x70(%rdi)
  401470:	65 72 61             	gs jb  4014d4 <_ZTI24CollectionSourceOperatorISt5tupleIJllllEEE+0x44>
  401473:	74 6f                	je     4014e4 <_ZTI24CollectionSourceOperatorISt5tupleIJllllEEE+0x54>
  401475:	72 00                	jb     401477 <_ZTS8Operator+0xa>
	...

0000000000401478 <_ZTI8Operator>:
  401478:	20 1b                	and    %bl,(%rbx)
  40147a:	60                   	(bad)  
  40147b:	00 00                	add    %al,(%rax)
  40147d:	00 00                	add    %al,(%rax)
  40147f:	00 6d 14             	add    %ch,0x14(%rbp)
  401482:	40 00 00             	add    %al,(%rax)
	...

0000000000401490 <_ZTI24CollectionSourceOperatorISt5tupleIJllllEEE>:
  401490:	a0 1c 60 00 00 00 00 	movabs 0x400000000000601c,%al
  401497:	00 40 
  401499:	14 40                	adc    $0x40,%al
  40149b:	00 00                	add    %al,(%rax)
  40149d:	00 00                	add    %al,(%rax)
  40149f:	00 78 14             	add    %bh,0x14(%rax)
  4014a2:	40 00 00             	add    %al,(%rax)
  4014a5:	00 00                	add    %al,(%rax)
  4014a7:	00 2c 20             	add    %ch,(%rax,%riz,1)
  4014aa:	00 73 74             	add    %dh,0x74(%rbx)
  4014ad:	64 3a 3a             	cmp    %fs:(%rdx),%bh
  4014b0:	67 65 74 3c          	addr32 gs je 4014f0 <__GNU_EH_FRAME_HDR+0x8>
  4014b4:	30 3e                	xor    %bh,(%rsi)
  4014b6:	28 72 65             	sub    %dh,0x65(%rdx)
  4014b9:	74 2e                	je     4014e9 <__GNU_EH_FRAME_HDR+0x1>
  4014bb:	76 61                	jbe    40151e <__GNU_EH_FRAME_HDR+0x36>
  4014bd:	6c                   	insb   (%dx),%es:(%rdi)
  4014be:	75 65                	jne    401525 <__GNU_EH_FRAME_HDR+0x3d>
  4014c0:	29 00                	sub    %eax,(%rax)
  4014c2:	3a 20                	cmp    (%rax),%ah
  4014c4:	00 63 6f             	add    %ah,0x6f(%rbx)
  4014c7:	6c                   	insb   (%dx),%es:(%rdi)
  4014c8:	6c                   	insb   (%dx),%es:(%rdi)
  4014c9:	65 63 74 69 6f       	movslq %gs:0x6f(%rcx,%rbp,2),%esi
  4014ce:	6e                   	outsb  %ds:(%rsi),(%dx)
  4014cf:	20 6f 70             	and    %ch,0x70(%rdi)
  4014d2:	0a 00                	or     (%rax),%al
  4014d4:	73 74                	jae    40154a <__GNU_EH_FRAME_HDR+0x62>
  4014d6:	64 3a 3a             	cmp    %fs:(%rdx),%bh
  4014d9:	67 65 74 3c          	addr32 gs je 401519 <__GNU_EH_FRAME_HDR+0x31>
  4014dd:	30 3e                	xor    %bh,(%rsi)
  4014df:	28 72 65             	sub    %dh,0x65(%rdx)
  4014e2:	73 29                	jae    40150d <__GNU_EH_FRAME_HDR+0x25>
	...

Disassembly of section .eh_frame_hdr:

00000000004014e8 <__GNU_EH_FRAME_HDR>:
  4014e8:	01 1b                	add    %ebx,(%rbx)
  4014ea:	03 3b                	add    (%rbx),%edi
  4014ec:	54                   	push   %rsp
  4014ed:	00 00                	add    %al,(%rax)
  4014ef:	00 09                	add    %cl,(%rcx)
  4014f1:	00 00                	add    %al,(%rax)
  4014f3:	00 88 f5 ff ff a0    	add    %cl,-0x5f00000b(%rax)
  4014f9:	00 00                	add    %al,(%rax)
  4014fb:	00 98 f6 ff ff 80    	add    %bl,-0x7f00000a(%rax)
  401501:	01 00                	add    %eax,(%rax)
  401503:	00 b8 f6 ff ff 70    	add    %bh,0x70fffff6(%rax)
  401509:	00 00                	add    %al,(%rax)
  40150b:	00 b8 f7 ff ff e8    	add    %bh,-0x17000009(%rax)
  401511:	00 00                	add    %al,(%rax)
  401513:	00 d8                	add    %bl,%al
  401515:	fc                   	cld    
  401516:	ff                   	(bad)  
  401517:	ff 20                	jmpq   *(%rax)
  401519:	01 00                	add    %eax,(%rax)
  40151b:	00 f8                	add    %bh,%al
  40151d:	fc                   	cld    
  40151e:	ff                   	(bad)  
  40151f:	ff                   	(bad)  
  401520:	38 01                	cmp    %al,(%rcx)
  401522:	00 00                	add    %al,(%rax)
  401524:	08 fd                	or     %bh,%ch
  401526:	ff                   	(bad)  
  401527:	ff 50 01             	callq  *0x1(%rax)
  40152a:	00 00                	add    %al,(%rax)
  40152c:	68 fe ff ff 98       	pushq  $0xffffffff98fffffe
  401531:	01 00                	add    %eax,(%rax)
  401533:	00 d8                	add    %bl,%al
  401535:	fe                   	(bad)  
  401536:	ff                   	(bad)  
  401537:	ff e0                	jmpq   *%rax
  401539:	01 00                	add    %eax,(%rax)
	...

Disassembly of section .eh_frame:

0000000000401540 <__FRAME_END__-0x1a0>:
  401540:	14 00                	adc    $0x0,%al
  401542:	00 00                	add    %al,(%rax)
  401544:	00 00                	add    %al,(%rax)
  401546:	00 00                	add    %al,(%rax)
  401548:	01 7a 52             	add    %edi,0x52(%rdx)
  40154b:	00 01                	add    %al,(%rcx)
  40154d:	78 10                	js     40155f <__GNU_EH_FRAME_HDR+0x77>
  40154f:	01 1b                	add    %ebx,(%rbx)
  401551:	0c 07                	or     $0x7,%al
  401553:	08 90 01 07 10 14    	or     %dl,0x14100701(%rax)
  401559:	00 00                	add    %al,(%rax)
  40155b:	00 1c 00             	add    %bl,(%rax,%rax,1)
  40155e:	00 00                	add    %al,(%rax)
  401560:	40 f6 ff             	idiv   %dil
  401563:	ff 2a                	ljmp   *(%rdx)
	...
  40156d:	00 00                	add    %al,(%rax)
  40156f:	00 14 00             	add    %dl,(%rax,%rax,1)
  401572:	00 00                	add    %al,(%rax)
  401574:	00 00                	add    %al,(%rax)
  401576:	00 00                	add    %al,(%rax)
  401578:	01 7a 52             	add    %edi,0x52(%rdx)
  40157b:	00 01                	add    %al,(%rcx)
  40157d:	78 10                	js     40158f <__GNU_EH_FRAME_HDR+0xa7>
  40157f:	01 1b                	add    %ebx,(%rbx)
  401581:	0c 07                	or     $0x7,%al
  401583:	08 90 01 00 00 24    	or     %dl,0x24000001(%rax)
  401589:	00 00                	add    %al,(%rax)
  40158b:	00 1c 00             	add    %bl,(%rax,%rax,1)
  40158e:	00 00                	add    %al,(%rax)
  401590:	e0 f4                	loopne 401586 <__GNU_EH_FRAME_HDR+0x9e>
  401592:	ff                   	(bad)  
  401593:	ff 00                	incl   (%rax)
  401595:	01 00                	add    %eax,(%rax)
  401597:	00 00                	add    %al,(%rax)
  401599:	0e                   	(bad)  
  40159a:	10 46 0e             	adc    %al,0xe(%rsi)
  40159d:	18 4a 0f             	sbb    %cl,0xf(%rdx)
  4015a0:	0b 77 08             	or     0x8(%rdi),%esi
  4015a3:	80 00 3f             	addb   $0x3f,(%rax)
  4015a6:	1a 3b                	sbb    (%rbx),%bh
  4015a8:	2a 33                	sub    (%rbx),%dh
  4015aa:	24 22                	and    $0x22,%al
  4015ac:	00 00                	add    %al,(%rax)
  4015ae:	00 00                	add    %al,(%rax)
  4015b0:	1c 00                	sbb    $0x0,%al
  4015b2:	00 00                	add    %al,(%rax)
  4015b4:	00 00                	add    %al,(%rax)
  4015b6:	00 00                	add    %al,(%rax)
  4015b8:	01 7a 50             	add    %edi,0x50(%rdx)
  4015bb:	4c 52                	rex.WR push %rdx
  4015bd:	00 01                	add    %al,(%rcx)
  4015bf:	78 10                	js     4015d1 <__GNU_EH_FRAME_HDR+0xe9>
  4015c1:	07                   	(bad)  
  4015c2:	03 30                	add    (%rax),%esi
  4015c4:	0b 40 00             	or     0x0(%rax),%eax
  4015c7:	03 1b                	add    (%rbx),%ebx
  4015c9:	0c 07                	or     $0x7,%al
  4015cb:	08 90 01 00 00 34    	or     %dl,0x34000001(%rax)
  4015d1:	00 00                	add    %al,(%rax)
  4015d3:	00 24 00             	add    %ah,(%rax,%rax,1)
  4015d6:	00 00                	add    %al,(%rax)
  4015d8:	c8 f6 ff ff          	enterq $0xfff6,$0xff
  4015dc:	1c 05                	sbb    $0x5,%al
  4015de:	00 00                	add    %al,(%rax)
  4015e0:	04 e4                	add    $0xe4,%al
  4015e2:	16                   	(bad)  
  4015e3:	40 00 42 0e          	add    %al,0xe(%rdx)
  4015e7:	10 42 0e             	adc    %al,0xe(%rdx)
  4015ea:	18 42 0e             	sbb    %al,0xe(%rdx)
  4015ed:	20 42 0e             	and    %al,0xe(%rdx)
  4015f0:	28 41 0e             	sub    %al,0xe(%rcx)
  4015f3:	30 47 0e             	xor    %al,0xe(%rdi)
  4015f6:	c0 01 83             	rolb   $0x83,(%rcx)
  4015f9:	06                   	(bad)  
  4015fa:	8c 05 8d 04 8e 03    	mov    %es,0x38e048d(%rip)        # 3ce1a8d <_end+0x36dfd9d>
  401600:	8f 02                	popq   (%rdx)
  401602:	00 00                	add    %al,(%rax)
  401604:	00 00                	add    %al,(%rax)
  401606:	00 00                	add    %al,(%rax)
  401608:	14 00                	adc    $0x0,%al
  40160a:	00 00                	add    %al,(%rax)
  40160c:	9c                   	pushfq 
  40160d:	00 00                	add    %al,(%rax)
  40160f:	00 b0 fb ff ff 14    	add    %dh,0x14fffffb(%rax)
	...
  40161d:	00 00                	add    %al,(%rax)
  40161f:	00 14 00             	add    %dl,(%rax,%rax,1)
  401622:	00 00                	add    %al,(%rax)
  401624:	b4 00                	mov    $0x0,%ah
  401626:	00 00                	add    %al,(%rax)
  401628:	b8 fb ff ff 09       	mov    $0x9fffffb,%eax
	...
  401635:	00 00                	add    %al,(%rax)
  401637:	00 2c 00             	add    %ch,(%rax,%rax,1)
  40163a:	00 00                	add    %al,(%rax)
  40163c:	cc                   	int3   
  40163d:	00 00                	add    %al,(%rax)
  40163f:	00 b0 fb ff ff 5f    	add    %dh,0x5ffffffb(%rax)
  401645:	01 00                	add    %eax,(%rax)
  401647:	00 00                	add    %al,(%rax)
  401649:	42 0e                	rex.X (bad) 
  40164b:	10 42 0e             	adc    %al,0xe(%rdx)
  40164e:	18 42 0e             	sbb    %al,0xe(%rdx)
  401651:	20 42 0e             	and    %al,0xe(%rdx)
  401654:	28 41 0e             	sub    %al,0xe(%rcx)
  401657:	30 83 06 8c 05 8d    	xor    %al,-0x72fa73fa(%rbx)
  40165d:	04 8e                	add    $0x8e,%al
  40165f:	03 8f 02 00 00 00    	add    0x2(%rdi),%ecx
  401665:	00 00                	add    %al,(%rax)
  401667:	00 14 00             	add    %dl,(%rax,%rax,1)
  40166a:	00 00                	add    %al,(%rax)
  40166c:	fc                   	cld    
  40166d:	00 00                	add    %al,(%rax)
  40166f:	00 10                	add    %dl,(%rax)
  401671:	f5                   	cmc    
  401672:	ff                   	(bad)  
  401673:	ff 20                	jmpq   *(%rax)
  401675:	00 00                	add    %al,(%rax)
  401677:	00 00                	add    %al,(%rax)
  401679:	41 0e                	rex.B (bad) 
  40167b:	10 00                	adc    %al,(%rax)
  40167d:	00 00                	add    %al,(%rax)
  40167f:	00 44 00 00          	add    %al,0x0(%rax,%rax,1)
  401683:	00 14 01             	add    %dl,(%rcx,%rax,1)
  401686:	00 00                	add    %al,(%rax)
  401688:	c8 fc ff ff          	enterq $0xfffc,$0xff
  40168c:	65 00 00             	add    %al,%gs:(%rax)
  40168f:	00 00                	add    %al,(%rax)
  401691:	42 0e                	rex.X (bad) 
  401693:	10 8f 02 42 0e 18    	adc    %cl,0x180e4202(%rdi)
  401699:	8e 03                	mov    (%rbx),%es
  40169b:	45 0e                	rex.RB (bad) 
  40169d:	20 8d 04 42 0e 28    	and    %cl,0x280e4204(%rbp)
  4016a3:	8c 05 48 0e 30 86    	mov    %es,-0x79cff1b8(%rip)        # ffffffff867024f1 <_end+0xffffffff86100801>
  4016a9:	06                   	(bad)  
  4016aa:	48 0e                	rex.W (bad) 
  4016ac:	38 83 07 4d 0e 40    	cmp    %al,0x400e4d07(%rbx)
  4016b2:	72 0e                	jb     4016c2 <__GNU_EH_FRAME_HDR+0x1da>
  4016b4:	38 41 0e             	cmp    %al,0xe(%rcx)
  4016b7:	30 41 0e             	xor    %al,0xe(%rcx)
  4016ba:	28 42 0e             	sub    %al,0xe(%rdx)
  4016bd:	20 42 0e             	and    %al,0xe(%rdx)
  4016c0:	18 42 0e             	sbb    %al,0xe(%rdx)
  4016c3:	10 42 0e             	adc    %al,0xe(%rdx)
  4016c6:	08 00                	or     %al,(%rax)
  4016c8:	14 00                	adc    $0x0,%al
  4016ca:	00 00                	add    %al,(%rax)
  4016cc:	5c                   	pop    %rsp
  4016cd:	01 00                	add    %eax,(%rax)
  4016cf:	00 f0                	add    %dh,%al
  4016d1:	fc                   	cld    
  4016d2:	ff                   	(bad)  
  4016d3:	ff 02                	incl   (%rdx)
	...

00000000004016e0 <__FRAME_END__>:
  4016e0:	00 00                	add    %al,(%rax)
	...

Disassembly of section .gcc_except_table:

00000000004016e4 <GCC_except_table0>:
  4016e4:	ff 03                	incl   (%rbx)
  4016e6:	5d                   	pop    %rbp
  4016e7:	03 5b 3c             	add    0x3c(%rbx),%ebx
  4016ea:	00 00                	add    %al,(%rax)
  4016ec:	00 57 00             	add    %dl,0x0(%rdi)
  4016ef:	00 00                	add    %al,(%rax)
  4016f1:	06                   	(bad)  
  4016f2:	05 00 00 00 cf       	add    $0xcf000000,%eax
  4016f7:	00 00                	add    %al,(%rax)
  4016f9:	00 ea                	add    %ch,%dl
  4016fb:	01 00                	add    %eax,(%rax)
  4016fd:	00 1a                	add    %bl,(%rdx)
  4016ff:	05 00 00 00 e1       	add    $0xe1000000,%eax
  401704:	02 00                	add    (%rax),%al
  401706:	00 9b 01 00 00 d4    	add    %bl,-0x2bffffff(%rbx)
  40170c:	04 00                	add    $0x0,%al
  40170e:	00 00                	add    %al,(%rax)
  401710:	81 04 00 00 0f 00 00 	addl   $0xf00,(%rax,%rax,1)
  401717:	00 04 05 00 00 00 95 	add    %al,-0x6b000000(,%rax,1)
  40171e:	04 00                	add    $0x0,%al
  401720:	00 05 00 00 00 1a    	add    %al,0x1a000000(%rip)        # 1a401726 <_end+0x19dffa36>
  401726:	05 00 00 00 9a       	add    $0x9a000000,%eax
  40172b:	04 00                	add    $0x0,%al
  40172d:	00 76 00             	add    %dh,0x0(%rsi)
  401730:	00 00                	add    %al,(%rax)
  401732:	00 00                	add    %al,(%rax)
  401734:	00 00                	add    %al,(%rax)
  401736:	00 10                	add    %dl,(%rax)
  401738:	05 00 00 05 00       	add    $0x50000,%eax
  40173d:	00 00                	add    %al,(%rax)
  40173f:	1a                   	.byte 0x1a
  401740:	05                   	.byte 0x5
  401741:	00 00                	add    %al,(%rax)
	...

Disassembly of section .init_array:

0000000000601748 <__frame_dummy_init_array_entry>:
  601748:	70 0c                	jo     601756 <__frame_dummy_init_array_entry+0xe>
  60174a:	40 00 00             	add    %al,(%rax)
  60174d:	00 00                	add    %al,(%rax)
  60174f:	00 80 0b 40 00 00    	add    %al,0x400b(%rax)
  601755:	00 00                	add    %al,(%rax)
	...

Disassembly of section .fini_array:

0000000000601758 <__do_global_dtors_aux_fini_array_entry>:
  601758:	50                   	push   %rax
  601759:	0c 40                	or     $0x40,%al
  60175b:	00 00                	add    %al,(%rax)
  60175d:	00 00                	add    %al,(%rax)
	...

Disassembly of section .jcr:

0000000000601760 <__JCR_END__>:
	...

Disassembly of section .dynamic:

0000000000601768 <_DYNAMIC>:
  601768:	01 00                	add    %eax,(%rax)
  60176a:	00 00                	add    %al,(%rax)
  60176c:	00 00                	add    %al,(%rax)
  60176e:	00 00                	add    %al,(%rax)
  601770:	01 00                	add    %eax,(%rax)
  601772:	00 00                	add    %al,(%rax)
  601774:	00 00                	add    %al,(%rax)
  601776:	00 00                	add    %al,(%rax)
  601778:	01 00                	add    %eax,(%rax)
  60177a:	00 00                	add    %al,(%rax)
  60177c:	00 00                	add    %al,(%rax)
  60177e:	00 00                	add    %al,(%rax)
  601780:	77 00                	ja     601782 <_DYNAMIC+0x1a>
  601782:	00 00                	add    %al,(%rax)
  601784:	00 00                	add    %al,(%rax)
  601786:	00 00                	add    %al,(%rax)
  601788:	01 00                	add    %eax,(%rax)
  60178a:	00 00                	add    %al,(%rax)
  60178c:	00 00                	add    %al,(%rax)
  60178e:	00 00                	add    %al,(%rax)
  601790:	69 00 00 00 00 00    	imul   $0x0,(%rax),%eax
  601796:	00 00                	add    %al,(%rax)
  601798:	0c 00                	or     $0x0,%al
  60179a:	00 00                	add    %al,(%rax)
  60179c:	00 00                	add    %al,(%rax)
  60179e:	00 00                	add    %al,(%rax)
  6017a0:	50                   	push   %rax
  6017a1:	0a 40 00             	or     0x0(%rax),%al
  6017a4:	00 00                	add    %al,(%rax)
  6017a6:	00 00                	add    %al,(%rax)
  6017a8:	0d 00 00 00 00       	or     $0x0,%eax
  6017ad:	00 00                	add    %al,(%rax)
  6017af:	00 c4                	add    %al,%ah
  6017b1:	13 40 00             	adc    0x0(%rax),%eax
  6017b4:	00 00                	add    %al,(%rax)
  6017b6:	00 00                	add    %al,(%rax)
  6017b8:	19 00                	sbb    %eax,(%rax)
  6017ba:	00 00                	add    %al,(%rax)
  6017bc:	00 00                	add    %al,(%rax)
  6017be:	00 00                	add    %al,(%rax)
  6017c0:	48 17                	rex.W (bad) 
  6017c2:	60                   	(bad)  
  6017c3:	00 00                	add    %al,(%rax)
  6017c5:	00 00                	add    %al,(%rax)
  6017c7:	00 1b                	add    %bl,(%rbx)
  6017c9:	00 00                	add    %al,(%rax)
  6017cb:	00 00                	add    %al,(%rax)
  6017cd:	00 00                	add    %al,(%rax)
  6017cf:	00 10                	add    %dl,(%rax)
  6017d1:	00 00                	add    %al,(%rax)
  6017d3:	00 00                	add    %al,(%rax)
  6017d5:	00 00                	add    %al,(%rax)
  6017d7:	00 1a                	add    %bl,(%rdx)
  6017d9:	00 00                	add    %al,(%rax)
  6017db:	00 00                	add    %al,(%rax)
  6017dd:	00 00                	add    %al,(%rax)
  6017df:	00 58 17             	add    %bl,0x17(%rax)
  6017e2:	60                   	(bad)  
  6017e3:	00 00                	add    %al,(%rax)
  6017e5:	00 00                	add    %al,(%rax)
  6017e7:	00 1c 00             	add    %bl,(%rax,%rax,1)
  6017ea:	00 00                	add    %al,(%rax)
  6017ec:	00 00                	add    %al,(%rax)
  6017ee:	00 00                	add    %al,(%rax)
  6017f0:	08 00                	or     %al,(%rax)
  6017f2:	00 00                	add    %al,(%rax)
  6017f4:	00 00                	add    %al,(%rax)
  6017f6:	00 00                	add    %al,(%rax)
  6017f8:	04 00                	add    $0x0,%al
  6017fa:	00 00                	add    %al,(%rax)
  6017fc:	00 00                	add    %al,(%rax)
  6017fe:	00 00                	add    %al,(%rax)
  601800:	40 02 40 00          	add    0x0(%rax),%al
  601804:	00 00                	add    %al,(%rax)
  601806:	00 00                	add    %al,(%rax)
  601808:	05 00 00 00 00       	add    $0x0,%eax
  60180d:	00 00                	add    %al,(%rax)
  60180f:	00 30                	add    %dh,(%rax)
  601811:	05 40 00 00 00       	add    $0x40,%eax
  601816:	00 00                	add    %al,(%rax)
  601818:	06                   	(bad)  
  601819:	00 00                	add    %al,(%rax)
  60181b:	00 00                	add    %al,(%rax)
  60181d:	00 00                	add    %al,(%rax)
  60181f:	00 f0                	add    %dh,%al
  601821:	02 40 00             	add    0x0(%rax),%al
  601824:	00 00                	add    %al,(%rax)
  601826:	00 00                	add    %al,(%rax)
  601828:	0a 00                	or     (%rax),%al
  60182a:	00 00                	add    %al,(%rax)
  60182c:	00 00                	add    %al,(%rax)
  60182e:	00 00                	add    %al,(%rax)
  601830:	7f 02                	jg     601834 <_DYNAMIC+0xcc>
  601832:	00 00                	add    %al,(%rax)
  601834:	00 00                	add    %al,(%rax)
  601836:	00 00                	add    %al,(%rax)
  601838:	0b 00                	or     (%rax),%eax
  60183a:	00 00                	add    %al,(%rax)
  60183c:	00 00                	add    %al,(%rax)
  60183e:	00 00                	add    %al,(%rax)
  601840:	18 00                	sbb    %al,(%rax)
  601842:	00 00                	add    %al,(%rax)
  601844:	00 00                	add    %al,(%rax)
  601846:	00 00                	add    %al,(%rax)
  601848:	15 00 00 00 00       	adc    $0x0,%eax
	...
  601855:	00 00                	add    %al,(%rax)
  601857:	00 03                	add    %al,(%rbx)
  601859:	00 00                	add    %al,(%rax)
  60185b:	00 00                	add    %al,(%rax)
  60185d:	00 00                	add    %al,(%rax)
  60185f:	00 60 19             	add    %ah,0x19(%rax)
  601862:	60                   	(bad)  
  601863:	00 00                	add    %al,(%rax)
  601865:	00 00                	add    %al,(%rax)
  601867:	00 02                	add    %al,(%rdx)
  601869:	00 00                	add    %al,(%rax)
  60186b:	00 00                	add    %al,(%rax)
  60186d:	00 00                	add    %al,(%rax)
  60186f:	00 68 01             	add    %ch,0x1(%rax)
  601872:	00 00                	add    %al,(%rax)
  601874:	00 00                	add    %al,(%rax)
  601876:	00 00                	add    %al,(%rax)
  601878:	14 00                	adc    $0x0,%al
  60187a:	00 00                	add    %al,(%rax)
  60187c:	00 00                	add    %al,(%rax)
  60187e:	00 00                	add    %al,(%rax)
  601880:	07                   	(bad)  
  601881:	00 00                	add    %al,(%rax)
  601883:	00 00                	add    %al,(%rax)
  601885:	00 00                	add    %al,(%rax)
  601887:	00 17                	add    %dl,(%rdi)
  601889:	00 00                	add    %al,(%rax)
  60188b:	00 00                	add    %al,(%rax)
  60188d:	00 00                	add    %al,(%rax)
  60188f:	00 e8                	add    %ch,%al
  601891:	08 40 00             	or     %al,0x0(%rax)
  601894:	00 00                	add    %al,(%rax)
  601896:	00 00                	add    %al,(%rax)
  601898:	07                   	(bad)  
  601899:	00 00                	add    %al,(%rax)
  60189b:	00 00                	add    %al,(%rax)
  60189d:	00 00                	add    %al,(%rax)
  60189f:	00 70 08             	add    %dh,0x8(%rax)
  6018a2:	40 00 00             	add    %al,(%rax)
  6018a5:	00 00                	add    %al,(%rax)
  6018a7:	00 08                	add    %cl,(%rax)
  6018a9:	00 00                	add    %al,(%rax)
  6018ab:	00 00                	add    %al,(%rax)
  6018ad:	00 00                	add    %al,(%rax)
  6018af:	00 78 00             	add    %bh,0x0(%rax)
  6018b2:	00 00                	add    %al,(%rax)
  6018b4:	00 00                	add    %al,(%rax)
  6018b6:	00 00                	add    %al,(%rax)
  6018b8:	09 00                	or     %eax,(%rax)
  6018ba:	00 00                	add    %al,(%rax)
  6018bc:	00 00                	add    %al,(%rax)
  6018be:	00 00                	add    %al,(%rax)
  6018c0:	18 00                	sbb    %al,(%rax)
  6018c2:	00 00                	add    %al,(%rax)
  6018c4:	00 00                	add    %al,(%rax)
  6018c6:	00 00                	add    %al,(%rax)
  6018c8:	fe                   	(bad)  
  6018c9:	ff                   	(bad)  
  6018ca:	ff 6f 00             	ljmp   *0x0(%rdi)
  6018cd:	00 00                	add    %al,(%rax)
  6018cf:	00 e0                	add    %ah,%al
  6018d1:	07                   	(bad)  
  6018d2:	40 00 00             	add    %al,(%rax)
  6018d5:	00 00                	add    %al,(%rax)
  6018d7:	00 ff                	add    %bh,%bh
  6018d9:	ff                   	(bad)  
  6018da:	ff 6f 00             	ljmp   *0x0(%rdi)
  6018dd:	00 00                	add    %al,(%rax)
  6018df:	00 03                	add    %al,(%rbx)
  6018e1:	00 00                	add    %al,(%rax)
  6018e3:	00 00                	add    %al,(%rax)
  6018e5:	00 00                	add    %al,(%rax)
  6018e7:	00 f0                	add    %dh,%al
  6018e9:	ff                   	(bad)  
  6018ea:	ff 6f 00             	ljmp   *0x0(%rdi)
  6018ed:	00 00                	add    %al,(%rax)
  6018ef:	00 b0 07 40 00 00    	add    %dh,0x4007(%rax)
	...

Disassembly of section .got:

0000000000601958 <.got>:
	...

Disassembly of section .got.plt:

0000000000601960 <_GLOBAL_OFFSET_TABLE_>:
  601960:	68 17 60 00 00       	pushq  $0x6017
	...
  601975:	00 00                	add    %al,(%rax)
  601977:	00 86 0a 40 00 00    	add    %al,0x400a(%rsi)
  60197d:	00 00                	add    %al,(%rax)
  60197f:	00 96 0a 40 00 00    	add    %dl,0x400a(%rsi)
  601985:	00 00                	add    %al,(%rax)
  601987:	00 a6 0a 40 00 00    	add    %ah,0x400a(%rsi)
  60198d:	00 00                	add    %al,(%rax)
  60198f:	00 b6 0a 40 00 00    	add    %dh,0x400a(%rsi)
  601995:	00 00                	add    %al,(%rax)
  601997:	00 c6                	add    %al,%dh
  601999:	0a 40 00             	or     0x0(%rax),%al
  60199c:	00 00                	add    %al,(%rax)
  60199e:	00 00                	add    %al,(%rax)
  6019a0:	d6                   	(bad)  
  6019a1:	0a 40 00             	or     0x0(%rax),%al
  6019a4:	00 00                	add    %al,(%rax)
  6019a6:	00 00                	add    %al,(%rax)
  6019a8:	e6 0a                	out    %al,$0xa
  6019aa:	40 00 00             	add    %al,(%rax)
  6019ad:	00 00                	add    %al,(%rax)
  6019af:	00 f6                	add    %dh,%dh
  6019b1:	0a 40 00             	or     0x0(%rax),%al
  6019b4:	00 00                	add    %al,(%rax)
  6019b6:	00 00                	add    %al,(%rax)
  6019b8:	06                   	(bad)  
  6019b9:	0b 40 00             	or     0x0(%rax),%eax
  6019bc:	00 00                	add    %al,(%rax)
  6019be:	00 00                	add    %al,(%rax)
  6019c0:	16                   	(bad)  
  6019c1:	0b 40 00             	or     0x0(%rax),%eax
  6019c4:	00 00                	add    %al,(%rax)
  6019c6:	00 00                	add    %al,(%rax)
  6019c8:	26 0b 40 00          	or     %es:0x0(%rax),%eax
  6019cc:	00 00                	add    %al,(%rax)
  6019ce:	00 00                	add    %al,(%rax)
  6019d0:	36 0b 40 00          	or     %ss:0x0(%rax),%eax
  6019d4:	00 00                	add    %al,(%rax)
  6019d6:	00 00                	add    %al,(%rax)
  6019d8:	46 0b 40 00          	rex.RX or 0x0(%rax),%r8d
  6019dc:	00 00                	add    %al,(%rax)
  6019de:	00 00                	add    %al,(%rax)
  6019e0:	56                   	push   %rsi
  6019e1:	0b 40 00             	or     0x0(%rax),%eax
  6019e4:	00 00                	add    %al,(%rax)
  6019e6:	00 00                	add    %al,(%rax)
  6019e8:	66 0b 40 00          	or     0x0(%rax),%ax
  6019ec:	00 00                	add    %al,(%rax)
	...

Disassembly of section .data:

00000000006019f0 <__data_start>:
	...

00000000006019f8 <__dso_handle>:
	...

Disassembly of section .bss:

0000000000601a00 <_ZSt4cerr@@GLIBCXX_3.4>:
	...

0000000000601b10 <_ZTVN10__cxxabiv117__class_type_infoE@@CXXABI_1.3>:
	...

0000000000601b80 <_ZSt4cout@@GLIBCXX_3.4>:
	...

0000000000601c90 <_ZTVN10__cxxabiv120__si_class_type_infoE@@CXXABI_1.3>:
	...

0000000000601ce8 <completed.7585>:
  601ce8:	00 00                	add    %al,(%rax)
	...

0000000000601cec <_ZStL8__ioinit>:
  601cec:	00 00                	add    %al,(%rax)
	...

Disassembly of section .comment:

0000000000000000 <.comment>:
   0:	47                   	rex.RXB
   1:	43                   	rex.XB
   2:	43 3a 20             	rex.XB cmp (%r8),%spl
   5:	28 55 62             	sub    %dl,0x62(%rbp)
   8:	75 6e                	jne    78 <_init-0x4009d8>
   a:	74 75                	je     81 <_init-0x4009cf>
   c:	20 35 2e 34 2e 30    	and    %dh,0x302e342e(%rip)        # 302e3440 <_end+0x2fce1750>
  12:	2d 36 75 62 75       	sub    $0x75627536,%eax
  17:	6e                   	outsb  %ds:(%rsi),(%dx)
  18:	74 75                	je     8f <_init-0x4009c1>
  1a:	31 7e 31             	xor    %edi,0x31(%rsi)
  1d:	36 2e 30 34 2e       	ss xor %dh,%cs:(%rsi,%rbp,1)
  22:	34 29                	xor    $0x29,%al
  24:	20 35 2e 34 2e 30    	and    %dh,0x302e342e(%rip)        # 302e3458 <_end+0x2fce1768>
  2a:	20 32                	and    %dh,(%rdx)
  2c:	30 31                	xor    %dh,(%rcx)
  2e:	36 30 36             	xor    %dh,%ss:(%rsi)
  31:	30 39                	xor    %bh,(%rcx)
  33:	00 55 62             	add    %dl,0x62(%rbp)
  36:	75 6e                	jne    a6 <_init-0x4009aa>
  38:	74 75                	je     af <_init-0x4009a1>
  3a:	20 63 6c             	and    %ah,0x6c(%rbx)
  3d:	61                   	(bad)  
  3e:	6e                   	outsb  %ds:(%rsi),(%dx)
  3f:	67 20 76 65          	and    %dh,0x65(%esi)
  43:	72 73                	jb     b8 <_init-0x400998>
  45:	69 6f 6e 20 33 2e 37 	imul   $0x372e3320,0x6e(%rdi),%ebp
  4c:	2e 31 2d 32 75 62 75 	xor    %ebp,%cs:0x75627532(%rip)        # 75627585 <_end+0x75025895>
  53:	6e                   	outsb  %ds:(%rsi),(%dx)
  54:	74 75                	je     cb <_init-0x400985>
  56:	32 20                	xor    (%rax),%ah
  58:	28 74 61 67          	sub    %dh,0x67(%rcx,%riz,2)
  5c:	73 2f                	jae    8d <_init-0x4009c3>
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
  74:	73 65                	jae    db <_init-0x400975>
  76:	64 20 6f 6e          	and    %ch,%fs:0x6e(%rdi)
  7a:	20 4c 4c 56          	and    %cl,0x56(%rsp,%rcx,2)
  7e:	4d 20 33             	rex.WRB and %r14b,(%r11)
  81:	2e 37                	cs (bad) 
  83:	2e 31 29             	xor    %ebp,%cs:(%rcx)
	...
