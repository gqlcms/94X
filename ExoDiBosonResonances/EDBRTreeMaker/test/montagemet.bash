#!/bin/bash
#rm Multi_*.png
idx=-1; unset img;
REGION=(SR1 SR2 SR3 SR4 SR5 SR6 SR7)
variable=(MET_et)
for var in ${variable[*]};do
	for r in ${REGION[*]};do
		filename=mu_${r}_/${var}.png	
		((idx++)); img[idx]=${filename};
	done
	montage -mode concatenate -tile 7x1 ${img[*]} ${var}.pdf;
	display ${var}.pdf &
done

