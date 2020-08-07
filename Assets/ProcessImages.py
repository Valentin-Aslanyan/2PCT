"""
Convert paint-made .png graphics files into raw binary, Win32 compatible
"""

from PIL import Image
import struct

#General Windows stuff
swap_red_blue=True #Apparently true on Windows
NBYTE_PX=4

#Menu buttons
MENU_BN_Width=150
MENU_BN_Height=50
names=["NewGameSelect","NewGameDeselect","ResumeSelect","ResumeDeselect","ResumeGrey"]
outfilename="MenuButtons.bin"
outfile=open(outfilename,"wb")
for fname in names:
	BN_image=Image.open(fname+'.png')
	BN_pixels=BN_image.load()
	width,height=BN_image.size
	if width==MENU_BN_Width and height==MENU_BN_Height:
		for idx_y in range(height):
			for idx_x in range(width):
				pixel_value=BN_pixels[idx_x,idx_y]
				if NBYTE_PX==3:
					if swap_red_blue:
						buf=struct.pack("BBB",pixel_value[2],pixel_value[1],pixel_value[0])
					else:
						buf=struct.pack("BBB",pixel_value[0],pixel_value[1],pixel_value[2])
				elif NBYTE_PX==4:
					if swap_red_blue:
						buf=struct.pack("BBBB",pixel_value[2],pixel_value[1],pixel_value[0],255)
					else:
						buf=struct.pack("BBBB",pixel_value[0],pixel_value[1],pixel_value[2],255)
				else:
					print("Error")
					buf=struct.pack("B",0)
				outfile.write(buf)
	else:
		print("Error: dimensions incorrect for: ",fname)
outfile.close()

def parse_single_image(infilename,outfilename,target_width,target_height):
	outfile=open(outfilename+".bin","wb")
	in_image=Image.open(infilename+'.png')
	in_pixels=in_image.load()
	width,height=in_image.size
	if width==target_width and height==target_height:
		for idx_y in range(height):
			for idx_x in range(width):
				pixel_value=in_pixels[idx_x,idx_y]
				if NBYTE_PX==3:
					if swap_red_blue:
						buf=struct.pack("BBB",pixel_value[2],pixel_value[1],pixel_value[0])
					else:
						buf=struct.pack("BBB",pixel_value[0],pixel_value[1],pixel_value[2])
				elif NBYTE_PX==4:
					if swap_red_blue:
						buf=struct.pack("BBBB",pixel_value[2],pixel_value[1],pixel_value[0],255)
					else:
						buf=struct.pack("BBBB",pixel_value[0],pixel_value[1],pixel_value[2],255)
				else:
					print("Error")
					buf=struct.pack("B",0)
				outfile.write(buf)
	else:
		print("Error: dimensions incorrect for: ",infilename)
	outfile.close()
	

#Instructions card
MENU_Inst_Width=550
MENU_Inst_Height=520
parse_single_image("Instructions","MenuInstructions",MENU_Inst_Width,MENU_Inst_Height)

#Digits for the score
DIGIT_WIDTH=15
DIGIT_HEIGHT=17
parse_single_image("Digits","Digits",DIGIT_WIDTH*10,DIGIT_HEIGHT)

#"Score" letters
SCORE_WIDTH=91
SCORE_HEIGHT=27
parse_single_image("Score","Score",SCORE_WIDTH,SCORE_HEIGHT)

#"Player Next Piece" letters
PLAYER_WIDTH=99
PLAYER_HEIGHT=38
parse_single_image("PlayerNext","PlayerNext",PLAYER_WIDTH,PLAYER_HEIGHT)



