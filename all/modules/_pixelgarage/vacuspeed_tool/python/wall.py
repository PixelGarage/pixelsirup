import copy
import sys
import time
import optparse

color = (255, 0, 0)
screen = None
ID = 4
J = 5
N = 6

wcolumns = 0
wrows = 0
wall = []

pcolumns = 0
prows = 0
pattern = []

outputFileName = ""

def displayData(rectangles, marginal, offsetx, offsety):

	for i in range(0, len(marginal)):
		x0 = (marginal[i][0])
		y0 = (marginal[i][1])
		x1 = (marginal[i][2])
		y1 = (marginal[i][3])
		w = x1 - x0
		h = y1 - y0		
		pygame.draw.rect(screen, (255, 255, 0), (offsetx + x0, offsety + y0, w - 1, h - 1))

	for i in range (0, len(rectangles), 4):
		x0 = rectangles[i]
		y0 = rectangles[i + 1]
		w = (rectangles[i + 2] - rectangles[i])
		h = (rectangles[i + 3] - rectangles[i + 1])
		p = w * h
		if (p < 25 * 25):
			pygame.draw.rect(screen, (245, 222, 179), (offsetx + x0, offsety + y0, w - 1, h - 1))
		elif (p < 25 * 50):
			pygame.draw.rect(screen, (222, 184, 135), (offsetx + x0, offsety + y0, w - 1, h - 1))
		elif (p < 50 * 50):
			pygame.draw.rect(screen, (210, 105, 30), (offsetx + x0, offsety + y0, w - 1, h - 1))
		elif (p < 50 * 100):
			pygame.draw.rect(screen, (255, 69, 0), (offsetx + x0, offsety + y0, w - 1, h - 1))
		else:
			pygame.draw.rect(screen, (255, 0, 0), (offsetx + x0, offsety + y0, w - 1, h - 1))
	
	pygame.display.flip()

def displayHoles(holes, offsetx, offsety):
	for (a, b, c, d) in holes:
		pygame.draw.rect(screen, (0, 255, 0), (offsetx + a, offsety +  b, (c - a) - 1, (d - b) - 1))
		pass
	pygame.display.flip()

def is2x2(a, i, j):
	rows = len(a)
	columns = len(a[0])

	if ((i + 1 < rows) and (j + 1 < columns) and a[i][j] == a[i + 1][j] == a[i][j + 1] == a[i + 1][j + 1]):
		return (a[i][j] != -1)

	return 0

def is2x1(a, i, j):	
	rows = len(a)
	columns = len(a[0])

	if ((i < rows) and (j + 1 < columns) and a[i][j] == a[i][j + 1]):
		return (a[i][j] != -1)

	return 0

def is1x2(a, i, j):
	rows = len(a)
	columns = len(a[0])

	if ((i + 1 < rows) and (j < columns) and a[i][j] == a[i + 1][j]):
		return (a[i][j] != -1)

	return 0

def existsTopLeftCross(a, i, j):
	rows = len(a)
	columns = len(a[0])

	counter = [a[i][j]]

	if ((i - 1 >= 0) and (j - 1 >= 0) and (i < rows) and (j < columns)):
		if ((a[i - 1][j] != -1) and (not a[i - 1][j] in counter)): counter += [a[i - 1][j]]
		if ((a[i - 1][j - 1] != -1) and (not a[i - 1][j - 1] in counter)): counter += [a[i - 1][j - 1]]
		if ((a[i][j - 1] != -1) and (not a[i][j - 1] in counter)): counter += [a[i][j - 1]]
		if (len(counter) == 4):
			return 1
	return 0

def existsTopRightCross(a, i, j):
	rows = len(a)
	columns = len(a[0])

	counter = [a[i][j]]

	if ((i - 1 >= 0) and (j + 1 < columns) and (i < rows) and (j < columns)):
		if ((a[i - 1][j] != -1) and (not a[i - 1][j] in counter)): counter += [a[i - 1][j]]
		if ((a[i - 1][j + 1] != -1) and (not a[i - 1][j + 1] in counter)): counter += [a[i - 1][j + 1]]
		if ((a[i][j + 1] != -1) and (not a[i][j + 1] in counter)): counter += [a[i][j + 1]]	
		if (len(counter) == 4):
			return 1
	return 0

def existsBottomLeftCross(a, i, j):
	rows = len(a)
	columns = len(a[0])
	counter = [a[i][j]]

	if ((i + 1 < rows) and (j - 1 >= 0) and (i < rows) and (j < columns)):
		if ((a[i + 1][j] != -1) and (not a[i + 1][j] in counter)): counter += [a[i + 1][j]]
		if ((a[i][j - 1] != -1) and (not a[i][j - 1] in counter)): counter += [a[i][j - 1]]
		if ((a[i + 1][j - 1] != -1) and (not a[i + 1][j - 1] in counter)): counter += [a[i + 1][j - 1]]
		if (len(counter) == 4):
			return 1
	return 0

def existsBottomRightCross(a, i, j):
	rows = len(a)
	columns = len(a[0])

	counter = [a[i][j]]

	if ((i + 1 < rows) and (j + 1 < columns) and (i < rows) and (j < columns)):
		if ((a[i + 1][j] != -1) and (not a[i + 1][j] in counter)): counter += [a[i + 1][j]]
		if ((a[i][j + 1] != -1) and (not a[i][j + 1] in counter)): counter += [a[i][j + 1]]
		if ((a[i + 1][j + 1] != -1) and (not a[i + 1][j + 1] in counter)): counter += [a[i + 1][j + 1]]
		if (len(counter) == 4):
			return 1
	return 0

def swap2x2x1x2(a, i, j):
	x = a[i][j + 2]
	y = a[i + 1][j + 2]

	a[i][j + 2] = a[i][j + 1]
	a[i + 1][j + 2] = a[i + 1][j + 1]

	a[i][j + 1] = a[i][j]
	a[i + 1][j + 1] = a[i + 1][j]

	a[i][j] = x
	a[i + 1][j] = y

def swap1x2x2x2(a, i, j):
	x = a[i][j]
	y = a[i + 1][j]

	a[i][j] = a[i][j + 1]
	a[i + 1][j] = a[i + 1][j + 1]

	a[i][j + 1] = a[i][j + 2]
	a[i + 1][j + 1] = a[i + 1][j + 2]

	a[i][j + 2] = x
	a[i + 1][j + 2] = y

def swap2x2x2x1(a, i, j):
	x = a[i + 2][j]
	y = a[i + 2][j + 1]

	a[i + 2][j] = a[i + 1][j]
	a[i + 2][j + 1] = a[i + 1][j + 1]

	a[i + 1][j] = a[i][j]
	a[i + 1][j + 1] = a[i][j + 1]

	a[i][j] = x
	a[i][j + 1] = y

def swap2x1x2x2(a, i, j):
	x = a[i][j]
	y = a[i][j + 1]

	a[i][j] = a[i + 1][j]
	a[i][j + 1] = a[i + 1][j + 1]

	a[i + 1][j] = a[i + 2][j]
	a[i + 1][j + 1] = a[i + 2][j + 1]

	a[i + 2][j] = x
	a[i + 2][j  + 1] = y

def join2x2(a, i, j):
	a[i][j + 1] = a[i][j]
	a[i + 1][j + 1] = a[i][j]
	a[i + 1][j] = a[i][j]

def join4x4(a, i, j, dir):
	if (dir == 0):
		a[i][j + 2] = a[i][j]
		a[i + 1][j + 2] = a[i][j]
		a[i][j + 3] = a[i][j]
		a[i + 1][j + 3] = a[i][j]
	else:
		a[i + 2][j] = a[i][j]
		a[i + 2][j + 1] = a[i][j]
		a[i + 3][j] = a[i][j]
		a[i + 3][j + 1] = a[i][j]



def evaluateScore(a):
	rectangles = []
	score = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

	rows = len(a)
	columns = len(a[0])

	d = {}
	g = {}
	cnt = -2
	for i in range(0, rows):
		for j in range(0, columns):
			if a[i][j] < 0:
				continue

			element = a[i][j]

#			perform horizontal full test
			if ((j + 3 < columns) and (i + 1 < rows)):
				count = a[i][j:j + 4].count(element) + a[i + 1][j:j + 4].count(element)
				if (count == 8):
					score[8] += 1
					a[i][j:j + 4] = [cnt, cnt, cnt, cnt]
					a[i + 1][j:j + 4] = [cnt, cnt, cnt, cnt]
					d[(i,j)] = [i, j, 100, 50]
					g[cnt] = 8
					cnt -= 1
					continue

#			perform vertical full test
			if ((j + 1 < columns) and (i + 3 < rows)):
				counter = [a[i][j], a[i][j + 1], a[i + 1][j], a[i + 1][j + 1], a[i + 2][j], a[i + 2][j + 1], a[i + 3][j], a[i + 3][j + 1]]
				count = counter.count(element)
				if (count == 8):
					score[8] += 1
					a[i][j] = a[i][j + 1] = a[i + 1][j] = a[i + 1][j + 1] = a[i + 2][j] = a[i + 2][j + 1] = a[i + 3][j] = a[i + 3][j + 1] = cnt
					d[(i,j)] = [i, j, 50, 100]
					g[cnt] = 8
					cnt -= 1					
					continue

#			perform 2x2 test
			if ((j + 1 < columns) and (i + 1 < rows)):
				counter = [a[i][j], a[i][j + 1], a[i + 1][j], a[i + 1][j + 1]]
				count = counter.count(element)
				if (count == 4):
					score[4] += 1
					a[i][j] = a[i][j + 1] = a[i + 1][j] = a[i + 1][j + 1] = cnt
					d[(i,j)] = [i, j, 50, 50]
					g[cnt] = 4
					cnt -= 1					
					continue

	for i in range(0, rows):
		for j in range(0, columns):
			if a[i][j] < 0:
				continue

			element = a[i][j]
			hCount = a[i][j:j + 4].count(element)
			counter = [a[i][j]]

			if (i + 3 < rows):
				hCount += a[i + 1][j:j + 4].count(element)				
				counter += [a[i + 1][j], a[i + 2][j], a[i + 3][j]]				
				if (j + 1 < columns):
					counter += [a[i][j + 1], a[i + 1][j + 1], a[i + 2][j + 1], a[i + 3][j + 1]]
			elif (i + 2 < rows):
				hCount += a[i + 1][j:j + 4].count(element)				
				counter += [a[i + 1][j], a[i + 2][j]]
				if (j + 1 < columns):
					counter += [a[i][j + 1], a[i + 1][j + 1], a[i + 2][j + 1]]
			elif (i + 1 < rows):				
				hCount += a[i + 1][j:j + 4].count(element)				
				counter += [a[i + 1][j]]		
				if (j + 1 < columns):
					counter += [a[i][j + 1], a[i + 1][j + 1]]
			else:
				if (j + 1 < columns):
					counter += [a[i][j + 1]]

			vCount = counter.count(element)

			if (vCount > hCount):
#				perform horizontal 1x2 test
				if (j + 1 < columns):
					if (a[i][j + 1] == element):
						score[2] += 1
						a[i][j] = a[i][j + 1] = cnt
						d[(i,j)] = [i, j, 50, 25]
						g[cnt] = 2
						cnt -= 1
						continue

#				perform vertical 1x2 test
				if (i + 1 < rows):
					if (a[i + 1][j] == element):
						score[2] += 1
						a[i + 1][j] = a[i][j] = cnt
						d[(i,j)] = [i, j, 25, 50]
						g[cnt] = 2
						cnt -= 1
						continue
			else:
#				perform vertical 1x2 test
				if (i + 1 < rows):
					if (a[i + 1][j] == element):
						score[2] += 1
						a[i + 1][j] = a[i][j] = cnt
						d[(i,j)] = [i, j, 25, 50]
						g[cnt] = 2						
						cnt -= 1
						continue	

#				perform horizontal 1x2 test
				if (j + 1 < columns):
					if (a[i][j + 1] == element):
						score[2] += 1
						a[i][j] = a[i][j + 1] = cnt
						d[(i,j)] = [i, j, 50, 25]
						g[cnt] = 2						
						cnt -= 1
						continue			
#			print "1x1"
			score[1] += 1
			a[i][j] = cnt
			g[cnt] = 1
			d[(i,j)] = [i, j, 25, 25]
			cnt -= 1

	#look for groups of 2x2 followed by 1x2 and determine whether there is an anomally
	skip = []
	for i in range(0, rows):
		for j in range(0, columns):
			if (a[i][j] == -1):
				continue

			if (i, j) in skip:
				continue

			#horizontal RL
			if (is2x2(a, i, j) and (g[a[i][j]] == 4) and is1x2(a, i, j + 2) and (g[a[i][j + 2]] == 2) and (existsTopRightCross(a, i, j + 1) or existsBottomRightCross(a, i + 1, j + 1))):
					swap2x2x1x2(a, i, j)
					skip += [(i,j), (i + 1, j), (i, j + 1), (i + 1, j + 1), (i, j + 2), (i + 1, j + 2)]
					d[(i, j)] = [i, j, 25, 50]
					d[(i, j + 1)] = [i, j + 1, 50, 50]
					d[(i, j + 2)] = []
					#add to rectangles here!
					continue

			#horizontal LR
			if (is1x2(a, i, j) and (g[a[i][j]] == 2) and is2x2(a, i, j + 1) and (g[a[i][j + 1]] == 4) and (existsTopRightCross(a, i, j) or existsBottomRightCross(a, i + 1, j))):
					swap1x2x2x2(a, i, j)
					skip += [(i,j), (i + 1, j), (i, j + 1), (i + 1, j + 1), (i, j + 2), (i + 1, j + 2)]
					d[(i, j)] = [i, j, 50, 50]
					d[(i, j + 1)] = []
					d[(i, j + 2)] = [i, j + 2, 25, 50]
					#add to rectangles here!
					continue

			#vertical UD
			if (is2x2(a, i, j) and (g[a[i][j]] == 4) and is2x1(a, i + 2, j) and (g[a[i + 2][j]] == 2) and (existsBottomLeftCross(a, i + 1, j) or existsBottomRightCross(a, i + 1, j + 1))):
					swap2x2x2x1(a, i, j)
					skip += [(i,j), (i + 1, j), (i, j + 1), (i + 1, j + 1), (i + 2, j), (i + 2, j + 1)]
					d[(i, j)] = [i, j, 50, 25]
					d[(i + 1, j)] = [i + 1, j, 50, 50]
					d[(i + 2, j)] = []
					#add to rectangles here!
					continue

			#vertical DU
			if (is2x1(a, i, j) and (g[a[i][j]] == 2) and is2x2(a, i + 1, j) and (g[a[i + 1][j]] == 4) and (existsBottomLeftCross(a, i, j) or existsBottomRightCross(a, i, j + 1))):
					swap2x1x2x2(a, i, j)
					skip += [(i,j), (i + 1, j), (i, j + 1), (i + 1, j + 1), (i + 2, j), (i + 2, j + 1)]
					d[(i, j)] = [i, j, 50, 50]
					d[(i + 1, j)] = []
					d[(i + 2, j)] = [i + 2, j, 50, 25]
					#add to rectangles here!

	skip = []

	#looks for groups of 2x1 and 2x1 next to each other or 1x2 and 1x2 next to each other and then joins them into 2x2
	#looks for groups of 2x2 and 2x2 next to each other and then joins them into 4x4
	for i in range(0, rows):
		for j in range(0, columns):
			if (a[i][j] == -1):
				continue

			if (i, j) in skip:
				continue

			#horizontal join
			if (is2x2(a, i, j) and is2x2(a, i, j + 2) and (g[a[i][j]] == 4) and (g[a[i][j + 2]] == 4)):
					g[a[i][j]] = 8
					g[a[i][j + 2]] = 0
					join4x4(a, i, j, 0)
					skip += [(i, j), (i + 1, j), (i, j + 1), (i + 1, j + 1), (i, j + 2), (i + 1, j + 2), (i, j + 3), (i + 1, j + 3)]
					#add to rectangles here!
					d[(i,j)] = [i, j, 100, 50]
					d[(i, j + 2)] = []
					score[4] -= 2
					score[8] += 1
					continue

			#vertical join
			if (is2x2(a, i, j) and is2x2(a, i + 2, j) and (g[a[i][j]] == 4) and (g[a[i + 2][j]] == 4)):
					g[a[i][j]] = 0
					g[a[i + 2][j]] = 8
					join4x4(a, i, j, 1)
					skip += [(i, j), (i + 1, j), (i + 2, j), (i + 3, j), (i, j + 1), (i + 1, j + 1), (i + 2, j + 1), (i + 3, j + 1)]
					#add to rectangles here!
					d[(i,j)] = [i, j, 50, 100]
					d[(i + 2, j)] = []
					score[4] -= 2
					score[8] += 1
					continue

			#horizontal join
			if (is2x1(a, i, j) and is2x1(a, i + 1, j) and (g[a[i][j]] == 2) and (g[a[i + 1][j]] == 2)):
					g[a[i][j]] = 4
					g[a[i + 1][j]] = 0
					join2x2(a, i, j)
					skip += [(i,j), (i + 1, j), (i, j + 1), (i + 1, j + 1)]
					#add to rectangles here!
					d[(i,j)] = [i, j, 50, 50]
					d[(i + 1, j)] = []
					score[4] += 1
					score[2] -= 2
					continue

			#vertical join
			if (is1x2(a, i, j) and is1x2(a, i, j + 1) and (g[a[i][j]] == 2) and (g[a[i][j + 1]] == 2)):
					g[a[i][j]] = 4
					g[a[i][j + 1]] = 0					
					join2x2(a, i, j)
					skip += [(i,j), (i, j + 1), (i + 1, j), (i + 1, j + 1)]
					#add to rectangles here!
					d[(i,j)] = [i, j, 50, 50]
					d[(i, j + 1)] = []
					score[4] += 1
					score[2] -= 2

					continue
	#count all anomalies	
	for i in range(0, rows):
		for j in range(0, columns):
			if (a[i][j] == -1):
				continue;

			if (existsTopLeftCross(a, i, j)):
				score[9] += 1 
				
	for (i, j) in d.keys():
		if (len(d[(i, j)]) > 0):
			rectangles += d[(i, j)]

	return (score, rectangles)

def generateHalfShiftFill(x0, y0, x1, y1):
	L = abs(x1 - x0)
	l = abs(y1 - y0)
	
	hcolumns = int(L) / 25
	remainderC = L - hcolumns * 25

	if (remainderC > 0):
		hcolumns += 1
	
	if (hcolumns % 4 != 0):
		hcolumns += 4 - (hcolumns % 4)

	if (hcolumns / 4 == 1): hcolumns += 4

	hrows = int(l) / 25
	remainderR = l - hrows * 25

	if (remainderR > 0):
		hrows += 1

	if (hrows % 4 != 0):
		hrows += 4 - (hrows % 4)

	if (hrows / 4 == 1): hrows += 4

	hpattern = []
	maxdim = int(max([hrows, hcolumns]))

	for i in range(0, maxdim):
		row = []
		for j in range(0, maxdim):
			row += [-1]
		hpattern += [row]

	vpattern = copy.deepcopy(hpattern)
	cnt = 0
	i = 0
	for i in range(0, maxdim, 2):
		j = 0
		while (j < maxdim):
			hpattern[i][j] = cnt
			if (i + 1 < maxdim):
				hpattern[i + 1][j] = cnt
			if (j + 1 < maxdim):
				hpattern[i][j + 1] = cnt
				if (i + 1 < maxdim):
					hpattern[i + 1][j + 1] = cnt

			if (j == 0 and i != 0 and ((i % 4 == 2) or (i % 4 == 3))):
				cnt += 1
				j += 2
				continue

			if (j + 3 < maxdim):
				hpattern[i][j + 3] = cnt
				hpattern[i][j + 2] = cnt
				hpattern[i][j + 1] = cnt
				if (i + 1 < maxdim):
					hpattern[i + 1][j + 3] = cnt
					hpattern[i + 1][j + 2] = cnt
					hpattern[i + 1][j + 1] = cnt
			elif (j + 2 < maxdim):
				hpattern[i][j + 2] = cnt
				hpattern[i][j + 1] = cnt
				if (i + 2 < maxdim):
					hpattern[i + 1][j + 2] = cnt
					hpattern[i + 1][j + 1] = cnt
			elif (j + 1 < maxdim):
				hpattern[i][j + 1] = cnt
				if (i + 1 < maxdim):
					hpattern[i + 1][j + 1] = cnt

			cnt += 1
			j += 4

	for i in range(0, maxdim):
		for j in range(0, maxdim):
			vpattern[j][i] = hpattern[i][j]

	return (hpattern, vpattern)

def generateCorridorFill(x0, y0, x1, y1):
	# 2
	# Horizontal and Vertical
	L = abs(x1 - x0)
	l = abs(y1 - y0)
	
	hcolumns = int(L) / 25
	remainderC = L - hcolumns * 25

	if (remainderC > 0):
		hcolumns += 1
	
	if (hcolumns % 4 != 0):
		hcolumns += 4 - (hcolumns % 4)

	if (hcolumns / 4 == 1): hcolumns += 4

	hrows = int(l) / 25
	remainderR = l - hrows * 25

	if (remainderR > 0):
		hrows += 1

	if (hrows % 4 != 0):
		hrows += 4 - (hrows % 4)

	if (hrows / 4 == 1): hrows += 4

	maxdim = max(hrows, hcolumns) + 8
	hpattern = []
	for i in range(0, maxdim):
		row = []
		for j in range(0, maxdim):
			row += [-1]
		hpattern += [row]

	vpattern = copy.deepcopy(hpattern)
	cnt = 0
	j = 0
	while (j < maxdim):
		#process vertical tiles column
		for i in range(0, maxdim, 4):
			hpattern[i][j] = cnt;
			if (j + 1 < maxdim):
				hpattern[i][j + 1] = cnt
			if (i + 3 < maxdim):
				hpattern[i + 1][j] = cnt;
				hpattern[i + 2][j] = cnt;
				hpattern[i + 3][j] = cnt;			
				if (j + 1 < maxdim):
					hpattern[i + 1][j + 1] = cnt;
					hpattern[i + 2][j + 1] = cnt;
					hpattern[i + 3][j + 1] = cnt;
			elif (i + 2 < maxdim):
				hpattern[i + 1][j] = cnt;
				hpattern[i + 2][j] = cnt;
				if (j + 1 < maxdim):
					hpattern[i + 1][j + 1] = cnt;
					hpattern[i + 2][j + 1] = cnt;
			elif (i + 1 < maxdim):
				hpattern[i + 1][j] = cnt;
				if (j + 1 < maxdim):
					hpattern[i + 1][j + 1] = cnt;
			cnt += 1

		j += 2
		if (j >= maxdim):
			continue

		#process horizontal tiles column
		i = 0
		while (i < maxdim):
			hpattern[i][j] = cnt;
			if (i + 1 < maxdim and not i == 0):
				hpattern[i + 1][j] = cnt
			if (j + 3 < maxdim):
				hpattern[i][j + 1] = cnt
				hpattern[i][j + 2] = cnt
				hpattern[i][j + 3] = cnt
				if (i + 1 < maxdim and not i == 0):
					hpattern[i + 1][j + 1] = cnt
					hpattern[i + 1][j + 2] = cnt
					hpattern[i + 1][j + 3] = cnt
			elif (j + 2 < maxdim):
				hpattern[i][j + 1] = cnt
				hpattern[i][j + 2] = cnt
				if (i + 1 < maxdim and not i == 0):
					hpattern[i + 1][j + 1] = cnt
					hpattern[i + 1][j + 2] = cnt				
			elif (j + 1 < maxdim):
				hpattern[i][j + 1] = cnt
				if (i + 1 < maxdim and not i == 0):
					hpattern[i + 1][j + 1] = cnt
			cnt += 1

			if (i == 0):
				i += 1
			else:
				i += 2
		j += 4

	for i in range(0, maxdim):
		for j in range(0, maxdim):
			vpattern[j][i] = hpattern[i][j]
	
	return (hpattern, vpattern)			

def generateQuarterShiftFill(x0, y0, x1, y1):
	# 2
	#Horizontal and Vertical
	L = abs(x1 - x0)
	l = abs(y1 - y0)
	
	hcolumns = int(L) / 25
	remainderC = L - hcolumns * 25

	if (remainderC > 0):
		hcolumns += 1
	
	if (hcolumns % 4 != 0):
		hcolumns += 4 - (hcolumns % 4)

	if (hcolumns / 4 == 1): hcolumns += 4

	hrows = int(l) / 25
	remainderR = l - hrows * 25

	if (remainderR > 0):
		hrows += 1

	if (hrows % 4 != 0):
		hrows += 4 - (hrows % 4)

	if (hrows / 4 == 1): hrows += 4

	maxdim = max(hrows, hcolumns) + 4
	hpattern = []
	for i in range(0, maxdim):
		row = []
		for j in range(0, maxdim):
			row += [-1]
		hpattern += [row]

	vpattern = copy.deepcopy(hpattern)
	cnt = 0
	i = 0
	for i in range(0, maxdim, 2):
		j = 0
		while (j < maxdim):
			hpattern[i][j] = cnt
			if (i + 1 < maxdim):
				hpattern[i + 1][j] = cnt
			if (j == 0 and i != 0 and i % 4 != 0):
				cnt += 1
				j += 1
				continue

			if (j + 3 < maxdim):
				hpattern[i][j + 3] = cnt
				hpattern[i][j + 2] = cnt
				hpattern[i][j + 1] = cnt
				if (i + 1 < maxdim):
					hpattern[i + 1][j + 3] = cnt
					hpattern[i + 1][j + 2] = cnt
					hpattern[i + 1][j + 1] = cnt
			elif (j + 2 < maxdim):
				hpattern[i][j + 2] = cnt
				hpattern[i][j + 1] = cnt
				if (i + 2 < maxdim):
					hpattern[i + 1][j + 2] = cnt
					hpattern[i + 1][j + 1] = cnt
			elif (j + 1 < maxdim):
				hpattern[i][j + 1] = cnt
				if (i + 1 < maxdim):
					hpattern[i + 1][j + 1] = cnt

			cnt += 1
			j += 4

	for i in range(0, maxdim):
		for j in range(0, maxdim):
			vpattern[j][i] = hpattern[i][j]

	return (hpattern, vpattern)

#vertical split
def splitRectangleV(r, hole):
	x0 = r[0]
	y0 = r[1]
	x1 = r[2]
	y1 = r[3]

	hx0 = hole[0]
	hy0 = hole[1]
	hx1 = hole[2]
	hy1 = hole[3]

	temp = []
	split = 0

	if (y0 <= hy0 < y1 <= hy1):
		split = 1
		if (y0 == hy0):
			temp += [[x0, y0, x1, y1]]
		else:
			temp += [[x0, y0, x1, hy0]]
			temp += [[x0, hy0, x1, y1]]
	elif (hy0 <= y0 < hy1 <= y1):
		split = 1
		if (y1 == hy1):
			temp += [[x0, y0, x1, y1]]
		else:
			temp += [[x0, y0, x1, hy1]]
			temp += [[x0, hy1, x1, y1]]
	elif (y0 <= hy0 < hy1 <= y1):
		split = 1
		if (y0 == hy0):
			temp += [[x0, y0, x1, hy1]]
			if (hy1 < y1):
				temp += [[x0, hy1, x1, y1]]
		else:
			temp += [[x0, y0, x1, hy0]]
			temp += [[x0, hy0, x1, hy1]]
			if (hy1 < y1):
				temp += [[x0, hy1, x1, y1]]
	elif (hy0 < y0 < y1 < hy1):
		temp += [[x0, y0, x1, y1]]

	return temp

#horizontal split
def splitRectangleH(r, hole):
	x0 = r[0]
	y0 = r[1]
	x1 = r[2]
	y1 = r[3]

	hx0 = hole[0]
	hy0 = hole[1]
	hx1 = hole[2]
	hy1 = hole[3]

	temp = []
	split = 0

	if (x0 <= hx0 < x1 <= hx1):
		split = 1
		if (x0 == hx0):
			temp += [[x0, y0, x1, y1]]
		else:
			temp += [[x0, y0, hx0, y1]]
			temp += [[hx0, y0, x1, y1]]
	elif (hx0 <= x0 < hx1 <= x1):
		split = 1
		if (x1 == hx1):
			temp += [[x0, y0, x1, y1]]
		else:
			temp += [[x0, y0, hx1, y1]]
			temp += [[hx1, y0, x1, y1]]
	elif (x0 <= hx0 < hx1 <= x1):
		split = 1
		if (x0 == hx0):
			temp += [[x0, y0, hx1, y1]]
			if (hx1 < x1):
				temp += [[hx1, y0, x1, y1]]
		else:
			temp += [[x0, y0, hx0, y1]]
			temp += [[hx0, y0, hx1, y1]]
			if (hx1 < x1):
				temp += [[hx1, y0, x1, y1]]
	elif (hx0 < x0 < x1 < hx1):
		temp += [[x0, y0, x1, y1]]

	return temp

def isOverlapping(r, h):
	x1 = r[0]
	y1 = r[1]
	x2 = r[2]
	y2 = r[3]

	hx1 = h[0]
	hy1 = h[1]
	hx2 = h[2]
	hy2 = h[3]

	res = (x1 >= hx2 or x2 <= hx1 or y2 <= hy1 or y1 >= hy2)

	return not res

def splitAll(a, b, c, d, holes):
	if (len(holes) % 4 != 0):
		print "Invalid number of coordinates for holes!"
		sys.exit(1)

	x0 = min(a, c)
	y0 = min(b, d)
	x1 = max(a, c)
	y1 = max(b, d)

	rectangles = [[x0, y0, x1, y1]]
	while (len(rectangles) > 0):
		r = rectangles.pop(0)			
		for i in range(0, len(holes), 4):
			hx0 = min(holes[i], holes[i + 2])
			hy0 = min(holes[i + 1], holes[i + 3])
			hx1 = max(holes[i], holes[i + 2])
			hy1 = max(holes[i + 1], holes[i + 3])

def generateSplitOrder(crtRectangle, hole, splitMode):
	x0 = crtRectangle[0]
	y0 = crtRectangle[1]
	x1 = crtRectangle[2]
	y1 = crtRectangle[3]

	hx0 = hole[0]
	hy0 = hole[1]
	hx1 = hole[2]
	hy1 = hole[3]

	if (x0 <= hx0 < hx1 <= x1):
		if (splitMode):
			return [splitRectangleH, splitRectangleV]
		else:
			return [splitRectangleV, splitRectangleH]
	elif (y0 <= hy0 < hy1 <= y1):
		if (splitMode):
			return [splitRectangleV, splitRectangleH]
		else:
			return [splitRectangleV, splitRectangleH]

	if (splitMode):
		return [splitRectangleH, splitRectangleV]
	else:
		return [splitRectangleV, splitRectangleH]

def splitIntoMarginalTiles(result, holes, splitMode):
	crt = 0
	while (crt < len(result)):
		crtRectangle = result[crt]
		increment = 1
		for hole in holes:
			if (isOverlapping(crtRectangle, hole)):
				splitOrder = generateSplitOrder(crtRectangle, hole, splitMode)
				firstSplit = splitOrder[0](crtRectangle, hole)
				for r in firstSplit:
					if (isOverlapping(r, hole)):
						secondSplit = splitOrder[1](r, hole)
						for m in secondSplit:
							if (isIdentical(m, hole) or (isInsideHole(m, hole))):
								secondSplit.remove(m)
						result += secondSplit
					else:
						result += [r]
				increment = 0
				result.pop(crt)
				break
		crt += increment

	return result

def rIntDiv(x1, num):
	counter = 0

	while (counter * num <= x1):
		counter += 1
		if (counter * num > x1):
			counter -= 1
			break
	
	return (x1 - counter * num)

def generateMarginalTiles(x0, y0, x1, y1, holes, mask, tilesMode):
	innerRectangle = []
	rectangles = []
	result = []

	if (rIntDiv(x1, 25) > 0):
		hx0 = x1 - rIntDiv(x1, 25)
		if (rIntDiv(y1, 25) > 0):
			hy0 = y1 - rIntDiv(y1, 25)
			rectangles += [[hx0, y0, x1, hy0], [x0, hy0, x1, y1]]
			innerRectangle = [x0, y0, hx0, hy0]
		else:
			rectangles += [[hx0, y0, x1, y1]]
			innerRectangle = [x0, y0, hx0, y1]
	else:
		if (rIntDiv(y1, 25) > 0):
			hy0 = y1 - rIntDiv(y1, 25)
			rectangles += [[x0, hy0, x1, y1]]
			innerRectangle = [x0, y0, x1, hy0]
		else:
			innerRectangle = [x0, y0, x1, y1]

	side = []

	#generate side tiles
	if (len(rectangles) > 0):
		side = splitIntoMarginalTiles(rectangles, holes, tilesMode)

	rectangles = []
	rth = {}
	for i in range(0, len(holes)):
		hx0 = min(holes[i][0], holes[i][2])
		hy0 = min(holes[i][1], holes[i][3])
		hx1 = max(holes[i][0], holes[i][2])
		hy1 = max(holes[i][1], holes[i][3])

		if (rIntDiv(hx0, 25) > 0):
			rx0 = hx0 - rIntDiv(hx0, 25)
		else:
			rx0 = hx0

		if (rIntDiv(hy0, 25) > 0):
			ry0 = hy0 - rIntDiv(hy0, 25)
		else:
			ry0 = hy0

		if (rIntDiv(hx1, 25) > 0):
			rx1 = hx1 + (25 - rIntDiv(hx1, 25))
			if (rx1 > innerRectangle[2]):
				rx1 = innerRectangle[2]
		else:
			rx1 = hx1

		if (rIntDiv(hy1, 25) > 0):
			ry1 = hy1 + (25 - rIntDiv(hy1, 25))
			if (ry1 > innerRectangle[3]):
				ry1 = innerRectangle[3]
		else:
			ry1 = hy1

		rectangles += [[rx0, ry0, rx1, ry1]]
		rth[(rx0, ry0, rx1, ry1)] = holes[i]

	h = copy.copy(holes)

	#generate other tiles
	result = []
	for r in rectangles:
		v = splitIntoMarginalTiles([r], h, tilesMode)

		if (len(v) > 0):
			result += v

		h.remove(rth[(r[0], r[1], r[2], r[3])])
		h += [r]

	if (len(side) > 0):
		result += side

	return result

def generateMask(x0, y0, x1, y1, holes):
	x = x0
	y = y0
	step = 25
	rows = 0
	columns = 0

	mask = []	
	while y + step <= y1:
		row = []
		x = x0		
		columns = 0
		while (x + step <= x1):
			row += [0]
			columns += 1
			x += step			
		if (x < x1):
			row += [1]
			columns += 1			
		rows += 1
		mask += [row]

		y += step
		if (y < y1 and y + step > y1):
			x = x0
			row = []
			while (x + step <= x1):
				x += step
				row += [1]
			if (x < x1):
				row += [1]
			rows += 1
			mask += [row]

	if (len(holes) != 0):
		for i in range(0, len(mask)):
			for j in range(0, len(mask[0])):
				for (a, b, c, d) in holes:
					y = i * 25;
					x = j * 25;
					#top margin or inside
					if (x >= a) and (x < c) and (y >= b) and (y < d):
						mask[i][j] = 1
					#left margin
					elif (x < a) and (abs(a - x) < 25) and (y > b) and (y < d):
						mask[i][j] = 1
					#corner left up or top margin
					elif (((x < a) and (abs(a - x) < 25)) or (x >= a)) and (x < c) and (y <= b) and (abs(b - y) < 25):
						mask[i][j] = 1
	return mask
def displayMatrix(matrix):
	for i in range(0, len(matrix)):
		for j in range(0, len(matrix[0])):
			print repr(matrix[i][j]).rjust(3),
		print ""
	print ""

def getBestScore(fill, mask):
	maskRows = len(mask)
	maskColumns = len(mask[0])

	allScores = []
	allRectangles = []

	fillRows = len(fill)
	fillColumns = len(fill[0])

	bestScore = []
	bestRectangle = []

	maxscore = 0
	minAnomalies = sys.maxint
	for i in range(0, fillRows - maskRows + 1):
		for j in range(0, fillColumns - maskColumns + 1):
			matrix = []
			for a in range(0, maskRows):
				matrixRow = []
				for b in range(0, maskColumns):
					if (mask[a][b] == 1):
						matrixRow += [-1]
					else:
						matrixRow += [fill[i + a][j + b]]
				matrix += [matrixRow]

			(score, rectangle) = evaluateScore(matrix)
			if (score[9] < minAnomalies):
				points = long(score[8]) << 30 | long(score[4]) << 20 | long(score[2]) << 10 | long(score[1])
				minAnomalies = score[9]
				maxscore = points
				del bestScore[:]
				del bestRectangle[:]
				bestScore = [points, score]
				bestRectangle = rectangle
			elif (score[9] == minAnomalies):
				points = long(score[8]) << 30 | long(score[4]) << 20 | long(score[2]) << 10 | long(score[1])
				if points > maxscore:
					maxscore = points
					del bestScore[:]
					del bestRectangle[:]
					bestScore = [points, score]
					bestRectangle = rectangle

			allScores += [score]
			allRectangles += [rectangle]

	return (bestScore, bestRectangle)

def getWidthHeight(x0, y0, x1, y1):
	return (abs(x1 - x0), abs(y1 - y0))

def runHalfShift(x0, y0, x1, y1, mask, allScores, allRectangles, allNames, includeH, includeV):
	#print "HALF SHIFT"
	(hFill, vFill) = generateHalfShiftFill(x0, y0, x1, y1)

	if includeH:
		(score, rectangle)  = getBestScore(hFill, mask)
		allScores += [copy.copy(score)]
		allRectangles += [copy.copy(rectangle)]
		allNames += ["HA"]
	
	if includeV:
		(score, rectangle)  = getBestScore(vFill, mask)
		allScores += [copy.copy(score)]
		allRectangles += [copy.copy(rectangle)]
		allNames += ["HA"]
	
def runCorridorShift(x0, y0, x1, y1, mask, allScores, allRectangles, allNames, includeH, includeV):
	#print "CORRIDOR SHIFT"
	(hCorridor, vCorridor) = generateCorridorFill(x0, y0, x1, y1)

	if includeH:
		(score, rectangle)  = getBestScore(hCorridor, mask)
		allScores += [copy.copy(score)]
		allRectangles += [copy.copy(rectangle)]
		allNames += ["CO"]

	if includeV:
		(score, rectangle)  = getBestScore(vCorridor, mask)
		allScores += [copy.copy(score)]
		allRectangles += [copy.copy(rectangle)]
		allNames += ["CO"]

def runQuarterShift(x0, y0, x1, y1, mask, allScores, allRectangles, allNames, includeH, includeV):
	#print "QUARTER SHIFT"
	(hShift, vShift) = generateQuarterShiftFill(x0, y0, x1, y1)

	if includeH:
		(score, rectangle)  = getBestScore(hShift, mask)
		allScores += [copy.copy(score)]
		allRectangles += [copy.copy(rectangle)]
		allNames += ["QS"]

	if includeV:
		(score, rectangle)  = getBestScore(vShift, mask)
		allScores += [copy.copy(score)]
		allRectangles += [copy.copy(rectangle)]
		allNames += ["QS"]

def transformToAbsoluteCoords(rect):
	res = []
	for i in range(0, len(rect), 4):
		res += [rect[i + 1] * 25, rect[i] * 25, rect[i + 1] * 25 + rect[i + 2], rect[i] * 25 + rect[i + 3]]
	
	return res

def runAlgorithm(x1, y1, holes, options, tilesMode):
	x0 = y0 = 0
	allScores = []
	allRectangles = []
	allNames = []	
	mask = generateMask(x0, y0, x1, y1, holes)

	includeH = 0
	includeV = 0
	(w, h) = getWidthHeight(x0, y0, x1, y1)
	if (w == h):
		includeH = 1
		includeV = 1
	elif (w > h):
		includeH = 1
	else:
		includeV = 1

	if ("HA" in options):
		runHalfShift(x0, y0, x1, y1, mask, allScores, allRectangles, allNames, includeH, includeV)
	
	if ("CO" in options):
		runCorridorShift(x0, y0, x1, y1, mask, allScores, allRectangles, allNames, includeH, includeV)

	if ("QU" in options):
		runQuarterShift(x0, y0, x1, y1, mask, allScores, allRectangles, allNames, includeH, includeV)

	maxPoints = -1
	maxScore = -1
	maxIDX = -1
	minAnomalies = sys.maxint
	for i in range(0, len(allScores)):
		score = allScores[i][1]
		if (score[9] < minAnomalies):
			minAnomalies = score[9]

	for i in range(0, len(allScores)):
		points = allScores[i][0]
		score = allScores[i][1]
		
		if ((minAnomalies == score[9]) and (points > maxPoints)):
			maxPoints = points
			maxScore = score
			maxIDX = i
	
	if (maxIDX == -1):
		sys.exit(1)

	if (tilesMode):
		marginalTiles = generateMarginalTiles(0, 0, x1, y1, holes, mask, tilesMode)
	else:
		holes.reverse()
		marginalTiles = generateMarginalTiles(0, 0, x1, y1, holes, mask, tilesMode)

	return (transformToAbsoluteCoords(allRectangles[maxIDX]), marginalTiles, maxScore)

def getNumber(s):
    try:
        val = float(s)
        return val
    except ValueError:
        return -1

def checkValues(mode, wall, holes):
	modeValues = ['HA', 'CO', 'QU']
	mhbExists = 0
	for i in mode:
		if i not in modeValues:
			print "Value ", i, " not allowed in -m values"
			sys.exit(1)
	
	if (len(wall) != 2):
		print "Invalid number of coordinates for wall. Expected 2 found ", len(wall)
		sys.exit(1)
	
	if (len(holes) % 4 != 0):
		print "Invalid number of coordinates for holes."
		sys.exit(1)
	
	for i in range(0, len(wall)):
		num = getNumber(wall[i])
		if (num < 0):
			print "Invalid wall coordinates."			
			sys.exit(1)
		wall[i] = num

	for i in range(0, len(holes)):
		num = getNumber(holes[i])
		if (num < 0):
			print "Invalid holes coordinates."
			sys.exit(1)
		holes[i] = num

def split(option, opt, value, parser):
  setattr(parser.values, option.dest, value.split(','))

def testMethods():
	a = [[1, 2, 2, 3], [1, 4, 4, 3], [5, 6, 6, 7], [5, 6, 6, 7], [8, 9, 9, 11], [8, 10, 10, 11]]
	displayMatrix(a)

	m = copy.deepcopy(a)
	swap2x2x1x2(m, 2, 1)
	displayMatrix(m)

	m = copy.deepcopy(a)
	swap1x2x2x2(m, 2, 0)
	displayMatrix(m)

	m = copy.deepcopy(a)
	swap2x2x2x1(m, 2, 1)
	displayMatrix(m)

	m = copy.deepcopy(a)
	swap2x1x2x2(m, 1, 1)
	displayMatrix(m)

def isIdentical(crtRectangle, hole):
	return (crtRectangle[0] == hole[0] and crtRectangle[1] == hole[1] and crtRectangle[2] == hole[2] and crtRectangle[3] == hole[3])

def isInsideHole(crtRectangle, hole):
	return (hole[0] <= crtRectangle[0] <= crtRectangle[2] <= hole[2] and hole[1] <= crtRectangle[1] <= crtRectangle[3] <= hole[3])

def TTV(width, height, c):
	res = []
	for r in c:
		res += [[r[0], height - r[3], r[2], height - r[1]]]
	
	return res

def TTH(width, height, c):
	res = []
	for r in c:
		res += [[width - r[2], r[1], width - r[0], r[3]]]
	
	return res

def TRH(width, height, c):
	res = []
	for i in range(0, len(c), 4):
		res += [width - c[i + 2], c[i + 1], width - c[i], c[i + 3]]
	
	return res

def TRV(width, height, c):
	res = []
	for i in range(0, len(c), 4):
		res += [c[i], height - c[i + 3], c[i + 2], height - c[i + 1]]
	
	return res

def THH(width, height, data):
	res = []
	for (a, b, c, d) in data:
		res += [(width - c, b, width - a, d)]

	return res

def THV(width, height, data):
	res = []
	for (a, b, c, d) in data:
		res += [(a, height - d, c, height - b)]

	return res

def trimHoles(width, height, holes):
	res = []
	for (a, b, c, d) in holes:
		if (a >= width):
			continue

		if (b >= height):
			continue
		c2 = c
		if (c2 >= width):
			c2 = width

		d2 = d
		if (d2 >= height):
			d2 = height

		res += [(a, b, c2, d2)]

	return res

def isHorizontal(tile):
	return (abs(tile[2] - tile[0]) > abs(tile[3] - tile[1]))

def intersects(a, b):
	result = []
	r = set(b).intersection(a)
	for i in r:
		result += [i]

	return result
	
def generateSegments(width, doubleLayering, start, coords):
	if (width <= 100):
		return [[width], [width]]

	layer1 = []
	layer2 = []
	while (width >= 100):
		width -= 100
		layer1 += [100]

	if (width > 0):
		layer1 += [width]

	if (doubleLayering):
		layer2 = copy.copy(layer1)

	s1 = start
	s2 = start	
	tilesCoords1 = [s1]
	tilesCoords2 = [s2]
	for i in range(0, len(layer1)):
		tilesCoords1 += [s1 + layer1[i]]
		s1 += layer1[i]
		tilesCoords2 += [s2 + layer1[len(layer1) - i - 1]]
		s2 += layer1[len(layer1) - i - 1]

	result1 = intersects(coords, tilesCoords1)

	result2 = intersects(coords, tilesCoords2)

	if (len(result2) < len(result1)):
		layer1.reverse()

	return [layer1, layer2]

def splitMarginalTilesHalf(tiles, allX, allY):
	i = 0
	result = []
	while (i < len(tiles)):
		tile = tiles[i]
		#horizontal tile
		if isHorizontal(tile):
			height = abs(tile[3] - tile[1])
			width = abs(tile[2] - tile[0])
			doubleLayered = (height > 12.5)
			segments = generateSegments(width, doubleLayered, tile[0], allX)
			
			layer1 = segments[0] 
			layer2 = segments[1]
			a = tile[0]
			b = tile[0]
			for j in range(0, len(layer1)):
				if (doubleLayered):
					result += [[a, tile[1], a + layer1[j], tile[1] + 12.5], [b, tile[1] + 12.5, b + layer2[j], tile[3]]]
					b += layer2[j]
				else:
					result += [[a, tile[1], a + layer1[j], tile[3]]]
				a += layer1[j]
		else:
			#vertical tile
			height = abs(tile[3] - tile[1])
			width = abs(tile[2] - tile[0])

			doubleLayered = (width > 12.5)
			segments = generateSegments(height, doubleLayered, tile[1], allY)

			layer1 = segments[0] 
			layer2 = segments[1]
			a = tile[1]
			b = tile[1]
			for j in range(0, len(layer1)):
				if (doubleLayered):
					result += [[tile[2] - 12.5, a, tile[2], a + layer1[j]], [tile[0], b, tile[2] - 12.5, b + layer2[j]]]
					b += layer2[j]
				else:
					result += [[tile[0], a, tile[2], a + layer1[j]]]
				a += layer1[j]
		i += 1

	return result

def getAllX(tiles):
	result = []

	for i in range(0, len(tiles), 4):
		if (tiles[i] not in result):
			result += [tiles[i]]

		if (tiles[i + 2] not in result):
			result += [tiles[i + 2]]

	return result

def getAllY(tiles):
	result = []

	for i in range(0, len(tiles), 4):
		if (tiles[i + 1] not in result):
			result += [tiles[i + 1]]

		if (tiles[i + 3] not in result):
			result += [tiles[i + 3]]

	return result

def writeOutput(fileName, data):

	if (len(data) % 3 != 0 and len(data) > 0):
		print("ERROR\n")
		print("Invalid result!\n")
		sys.exit(1)

	print("SUCCESS\n");

	for i in range(0, len(data), 3):
		layer = data[i]
		mT = data[i + 1]
		maxScore = data[i + 2]
		print("\nLAYER " + str(i) + ":\n")
		print("SCORE: Total, 100x50, 50x50, 50x25, 25x25, marginal\n")
		print(str(maxScore[10] + maxScore[8] + maxScore[4] + maxScore[2] + maxScore[1]))
		print(" " + str(maxScore[8]) + " " + str(maxScore[4]) + " " + str(maxScore[2]) + " " + str(maxScore[1]) + " " + str(maxScore[10]) + "\n")
		print("COORDS: type, x0, y0, x1, y1\n")
		for j in range(0, len(layer), 4):
			if (((abs(layer[j] - layer[j + 2]) == 100) and (abs(layer[j + 1] - layer[j + 3]) == 50)) or ((abs(layer[j] - layer[j + 2]) == 50) and (abs(layer[j + 1] - layer[j + 3]) == 100))):
				print("100x50 " + str(layer[j]) + " " + str(layer[j + 1]) + " " + str(layer[j + 2]) + " " + str(layer[j + 3]) + "\n")
			elif ((abs(layer[j] - layer[j + 2]) == 50) and (abs(layer[j + 1] - layer[j + 3]) == 50)):
				print("50x50 " + str(layer[j]) + " " + str(layer[j + 1]) + " " + str(layer[j + 2]) + " " + str(layer[j + 3]) + "\n")
			elif (((abs(layer[j] - layer[j + 2]) == 50) and (abs(layer[j + 1] - layer[j + 3]) == 25)) or ((abs(layer[j] - layer[j + 2]) == 25) and (abs(layer[j + 1] - layer[j + 3]) == 50))):
				print("50x25 " + str(layer[j]) + " " + str(layer[j + 1]) + " " + str(layer[j + 2]) + " " + str(layer[j + 3]) + "\n")
			else:
				print("25x25 " + str(layer[j]) + " " + str(layer[j + 1]) + " " + str(layer[j + 2]) + " " + str(layer[j + 3]) + "\n")
		for j in range(0, len(mT)):
			m = mT[j]
			print("M " + str(m[0]) + " " + str(m[1]) + " " + str(m[2]) + " " + str(m[3]) + "\n")

def main():	
	global screen, outputFileName

	use = "usage: python %prog [options].file\nExample: python %prog -m HA,QU -w 500,500 -g 0,0,50,50 hash21311.txt"
	parser = optparse.OptionParser(usage=use)
	parser.add_option('-m', '--mode', type = 'string', action = 'callback', help = "Coma separated list, denoting tiling patterns to be used. Available values are: HA - half shift, CO - corridor fill, QU - quarter shift. Usage example: -m HA,QU", callback = split)
	parser.add_option('-w', '--wall', type = 'string', action = 'callback', help = "Coma separated list, denoting top left and bottom right wall coordinates. Usage example: -w 0,0,100,100", callback = split)
	parser.add_option('-g', '--gray', type = 'string', action = 'callback', help = "Coma separated list, denoting top left and bottom right gray zones coordinates. Each group of 4 points corresponds to a gray zone in the wall. Usage example for 2 gray zones: -g 0,0,20,30,50,50,60,65", callback = split)
	parser.add_option('-f', '--file', type = 'string', help = "Specifies the file name that the output is written to")
	parser.add_option('-d', '--display', type = 'string', help = "Specifies whether graphics will be displayed; used for debugging purpose. Values are: yes no")
	options, args = parser.parse_args()

	if (not options.file):
		parser.error("--file argumet not given")
	else:
		outputFileName = copy.copy(options.file)

	if (not options.mode):
		parser.error("--mode tiling not given")
	else:
		mode = copy.copy(options.mode)

	if (not options.wall):
		parser.error("--wall coordinates not given")
	else:
		wall = copy.copy(options.wall)

	if (not options.gray):
		rawHoles = []
	else:
		rawHoles = copy.copy(options.gray)

	if (not options.display):
		dData = False
	else:
		if (options.display.upper() == "YES"):
			dData = True
		else:
			dData = False

	checkValues(mode, wall, rawHoles)
	
	holes = []
	for i in range(0, len(rawHoles), 4):
		holes += [(rawHoles[i], rawHoles[i + 1], rawHoles[i + 2], rawHoles[i + 3])]

	expandedRight = rIntDiv(wall[0], 25)
	expandedDown = rIntDiv(wall[1], 25)
	
	eR = [0, 0]
	eD = [0, 0]
	layer = [[], []]
	marginalTiles = [[], []]
	rightTiles = [[], []]
	downTiles = [[], []]
	maxScore = [[], []]

	firstRoundFlip = 0

	if (expandedRight <= 12.5):
		eR[0] = expandedRight
		eR[1] = 0
	else:
		firstRoundFlip = 1
		eR[0] = 12.5
		eR[1] = expandedRight - 12.5

	if (expandedDown <= 12.5):
		eD[0] = expandedDown
		eD[1] = 0
	else:
		firstRoundFlip = 1
		eD[0] = 12.5
		eD[1] = expandedDown - 12.5

	if (expandedRight == 0):
		eR[1] = 12.5

	if (expandedDown == 0):
		eD[1] = 12.5

	oldX = wall[0]
	oldY = wall[1]

	#regular fill
	trimmedHoles = copy.copy(holes)
	for i in range(0, 2):		
		width = wall[0]
		height = wall[1]
		rightTiles = []
		downTiles = []
		if (firstRoundFlip or (i != 0)):
			if (eR[i] > 0):
				width = wall[0] - eR[i]			
				trimmedHoles = trimHoles(width, height, trimmedHoles)
				rightTiles = splitIntoMarginalTiles([[width, 0, width + eR[i], height]], holes, i - 1)

			if (eD[i] > 0):
				height = wall[1] - eD[i]
				trimmedHoles = trimHoles(width, height, trimmedHoles)
				downTiles = splitIntoMarginalTiles([[0, height, width, height + eD[i]]], holes, i - 1)

			(layer[i], marginalTiles[i], maxScore[i]) = runAlgorithm(width, height, THV(width, height, THH(width, height, trimmedHoles)), mode, 1 - i)

			layer[i] = copy.copy(TRV(width, height, TRH(width, height, layer[i])))
			marginalTiles[i] = copy.copy(TTV(width, height, TTH(width, height, marginalTiles[i])))

			if (len(rightTiles) > 0):
				marginalTiles[i] += copy.copy(rightTiles)

			if (len(downTiles) > 0):
				marginalTiles[i] += copy.copy(downTiles)
		else:
			(layer[i], marginalTiles[i], maxScore[i]) = runAlgorithm(width, height, holes, mode, 1 - i)

	allX = getAllX(layer[0])
	allY = getAllY(layer[0])

	marginalTiles[0] = splitMarginalTilesHalf(marginalTiles[0], allX, allY)

	allX = getAllX(layer[1])
	allY = getAllY(layer[1])
	marginalTiles[1] = splitMarginalTilesHalf(marginalTiles[1], allX, allY)

	maxScore[0] += [len(marginalTiles[0])]
	maxScore[1] += [len(marginalTiles[1])]	

	if (dData):
		pygame.init() 	
		screen = pygame.display.set_mode((1500, 800))

		displayData(layer[0], marginalTiles[0], 0, 0)
		displayHoles(holes, 0, 0)

		displayData(layer[1], marginalTiles[1], oldX + 20, 0)
		displayHoles(holes, oldX + 20, 0)

		while 1:
			Geesh = pygame.event.get()	
			if len(Geesh) > 0:	
				if ((Geesh[0].type == KEYDOWN) or (Geesh[0].type == QUIT)) : break;

	return [layer[0], marginalTiles[0], maxScore[0], layer[1], marginalTiles[1], maxScore[1]]

try:
	data = main()
	writeOutput(outputFileName, data)
except Exception, e:
	print("ERROR\n")
	print("Invalid result!\n")
	sys.exit(1)
	#f = open(outputFileName, "w")
	#f.write("ERROR\n")
	#traceback.print_exc(file=f)
	#f.close()
