#!/usr/bin/python
# -*- coding: <UTF-8> -*-
import re
import itertools, copy


class Literal:
    def __init__(self):
        self.fname = ""  # string, function name
        self.args = []  # arguement  lowercase:value uppercase:constant
        self.ifpos = True

    def AtomtoLiteral(self, atom):
        literal = Literal()
        literal.ifpos = False if atom[0] == '~' else True
        temp = list(filter(None, re.split('~|[(]|[)]|,', atom)))  # split the word and ignore empty
        literal.fname = temp[0]
        literal.args = temp[1:]  # if use \w, it will add two '' in the list
        return literal


class Sentence:
    def __init__(self):
        self.literal_list = []  # list of class Literal

    def Linetosentence(self, line):
        line = ''.join(line.split())
        sentence = Sentence()
        temp = re.split('\|', line)
        for atom in re.split('[\|]', line):  # take in space
            sentence.literal_list.append(Literal().AtomtoLiteral(atom))
        return sentence

    def Is_empty(self):
        return True if len(self.literal_list) == 0 else False

    def FirstLiteral_empty(self):
        return True if len(self.literal_list) == 1 and self.literal_list[0].fname == '' \
                       and self.literal_list[0].args == [] and self.literal_list[0].ifpos == True else False

    def Print_sen(self):
        for lit in self.literal_list:
            if lit.ifpos == False: print('~', end="")
            print(lit.fname + '(' + ','.join([arg for arg in lit.args]) + ')', end="")
            if lit is not self.literal_list[-1]: print('|', end="")
        print()

    def delduplicate(self):
        nodu = Sentence()
        for lit in self.literal_list:
            flag = True
            for o in nodu.literal_list:
                if o.fname == lit.fname and o.args == lit.args and o.ifpos == lit.ifpos:
                    flag = False
            if flag == True:
                nodu.literal_list.append(lit)
        return nodu

class Theta:
    def __init__(self):
        self.dict = {}
        self.failure = False


class KB:
    def __init__(self):
        self.sentence_list = []
        self.snum = 0

    def AddsentoKB(self, sentence):
        self.sentence_list.append(sentence)

    def Seperate_arg(self):
        for i, sen in enumerate(self.sentence_list):
            for lit in sen.literal_list:
                for j in range(len(lit.args)):
                    if lit.args[j][0].islower():
                        lit.args[j] += str(i)

    def Isnot_unified(self, sentence):
        for atom in sentence.literal_list:
            for lit in atom.args:
                if not lit.islower():  # if has uppercase
                    return False
        return True

    def Isliteral(self, sentence):
        return True if len(sentence.literal_list) == 1 else False

    def Flit(self, sentence):  # First literal
        return sentence.literal_list[0]

    def Is_VARIABLE(self, x):
        if isinstance(x, list):
            return False
        else:
            return True if x[0].islower() else False

    def Is_LIST(self, x):
        return True if isinstance(x, list) and len(x) != 1 else False

    def UNIFICATION(self, sen1: Sentence, sen2: Sentence):
        i = 0
        j = 0
        theta = Theta()
        flag = True
        for i in range(len(sen1.literal_list)):
            if flag == True:
                for j in range(len(sen2.literal_list)):
                    if flag == True:
                        if sen1.literal_list[i].fname == sen2.literal_list[j].fname and sen1.literal_list[i].ifpos != \
                                sen2.literal_list[j].ifpos:
                            flag = False
                            self.UNIFY(sen1.literal_list[i].args, sen2.literal_list[j].args, theta)
        if not theta.dict: return
        for i1 in range(len(sen1.literal_list)):
            for j1 in range(len(sen1.literal_list[i1].args)):
                if sen1.literal_list[i1].args[j1] in theta.dict:
                    sen1.literal_list[i1].args[j1] = theta.dict[sen1.literal_list[i1].args[j1]]

        for i2 in range(len(sen2.literal_list)):
            for j2 in range(len(sen2.literal_list[i2].args)):
                if sen2.literal_list[i2].args[j2] in theta.dict:
                    sen2.literal_list[i2].args[j2] = theta.dict[sen2.literal_list[i2].args[j2]]

    def UNIFY(self, x, y, theta: Theta):
        if isinstance(x, list) and len(x) == 1:
            x = x[0]
        if isinstance(y, list) and len(y) == 1:
            y = y[0]

        if theta.failure == True:
            return theta
        elif x == y:
            return theta
        elif self.Is_VARIABLE(x):
            return self.UNIFY_VAR(x, y, theta)
        elif self.Is_VARIABLE(y):
            return self.UNIFY_VAR(y, x, theta)
        elif self.Is_LIST(x) and self.Is_LIST(y):
            return self.UNIFY(x[1:], y[1:], self.UNIFY(x[0], y[0], theta))
            # return self.UNIFY(x.args[1:],y.args[1:],self.UNIFY(x.args[0],y.args[0],theta))
        else:
            theta.dict.clear()
            return theta

    def UNIFY_VAR(self, var, x, theta):
        if var in theta.dict:  # maybe also need check in dict.value()
            return self.UNIFY(theta.dict[var], x, theta)
        elif x in theta.dict:  # maybe here only find x from key, not value so useless
            return self.UNIFY(var, theta.dict[x], theta)
        else:
            theta.dict.update({var: x})
            return theta

    def Ifcontradiction(self, sentence: Sentence):
        for sen in self.sentence_list:
            if not self.Isliteral(sen) or self.Isnot_unified(sen):
                break
            else:
                if self.Flit(sentence).fname == self.Flit(sen).fname and \
                                self.Flit(sentence).args == self.Flit(sen).args and \
                                self.Flit(sentence).ifpos != self.Flit(sen).ifpos:
                    return True
        return False

    def FOL_RESOLVE(self, sen1: Sentence, sen2: Sentence):
        s1 = copy.deepcopy(sen1)
        s2 = copy.deepcopy(sen2)

        self.UNIFICATION(s1, s2)

        res = Sentence()
        for indexi, i in enumerate(s1.literal_list):
            for indexj, j in enumerate(s2.literal_list):
                if i.fname == j.fname and \
                                i.args == j.args and \
                                i.ifpos != j.ifpos:
                    del s1.literal_list[indexi], s2.literal_list[indexj]
                    if len(s1.literal_list) == len(s2.literal_list) == 0:
                        res.literal_list.append(Literal())
                        return res  # return empty literal but not nothing
                    res.literal_list.extend(s1.literal_list)
                    res.literal_list.extend(s2.literal_list)
                    res = res.delduplicate()
                    return res

    def equal_sen(self, sen1, sen2):
        sum = 0
        if len(sen1.literal_list) != len(sen2.literal_list): return False
        for pair in itertools.product(sen1.literal_list, sen2.literal_list):
            if pair[0].fname == pair[1].fname and pair[0].args == pair[1].args and pair[0].ifpos == pair[1].ifpos:
                sum += 1
        return True if sum == len(sen1.literal_list) else False

    def contains(self, small: list, large: list):
        sum = 0
        for pair in itertools.product(small, large):
            if self.equal_sen(pair[0], pair[1]):
                sum += 1
        return True if sum == len(small) else False

    def Unloop(self, sen1, sen2):
        sum = set()
        for pair in itertools.product(sen1.literal_list, sen2.literal_list):
            if pair[0].fname == pair[1].fname and pair[0].ifpos != pair[1].ifpos:
                sum.add(pair[0].fname)
            if len(sum) >= 2: return False
        return True

    def FOL_RESOLUTION(self, alpha: Sentence):
        alpha.literal_list[0].ifpos = not alpha.literal_list[0].ifpos
        self.AddsentoKB(alpha)  # add ~alpha into KB
        # clause == self.sentence_list
        hashmap = []
        haschange = True  # see if should stop
        new = []  # sentence list
        while len(self.sentence_list)<1000:
            for pair in itertools.combinations(self.sentence_list, 2):
                resolvents = None

                if hash(pair) not in hashmap and self.Unloop(*pair):  # if this two sentence has not been resolved and no loop
                    resolvents = self.FOL_RESOLVE(*pair)
                    hashmap.append(hash(pair))

                if resolvents is None: continue  # can't resolve, so return None
                if not resolvents.Is_empty() and resolvents.FirstLiteral_empty():
                    return True  # empty clause, means having contradiction
                if not resolvents.Is_empty() and not self.contains([resolvents], new):
                    # resolvents.Spe_arg_sen()
                    new.append(resolvents)
            if self.contains(new, self.sentence_list): return False  # this funcion may have bug!!!!!!!!!!!
            for sen in new:
                if not self.contains([sen], self.sentence_list):
                    self.sentence_list.append(sen)
        return False
# main

# read in input
f = open('input.txt')

pronum = int(f.readline())  # problem num
newKB = KB()
#
while pronum > 0:
    line = f.readline()
    newKB.AddsentoKB(Sentence().Linetosentence(line))
    pronum -= 1
newKB.Seperate_arg()

sennum = int(f.readline())  # sentence num
realKB = KB()
#
while sennum > 0:
    line = f.readline()
    realKB.AddsentoKB(Sentence().Linetosentence(line))
    sennum -= 1
realKB.Seperate_arg()

# output
f2 = open('output.txt', 'w')
for sen in newKB.sentence_list:
    f2.writelines(str(copy.deepcopy(realKB).FOL_RESOLUTION(sen)).upper() + '\n')
f2.close()
