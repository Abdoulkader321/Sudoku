#!/bin/bash
#Pour lancer le test, donnez lui un droit d'excecution, et renseignez les "path" vers vos répertoires.
#Your sudoku project folder
PROJECT_PATH="./sudoku"
#Stores the previous working directory
PREVIOUS_PWD=${PWD}
ERROR_FILE_TEST=${PREVIOUS_PWD}/stderr_file.txt

#Your test folders
#Seul TEST_PATH est nécéssaire pour les tests homework 1 et 2
TEST_PATH="./tests/grid-parser/"
TEST_PATH2="/home/.../.../.../Sudoku/test/grid-solver_2/"

ERR=0
SUCC=0
EXIT_SUCCESS=0
EXIT_FAILURE=1
VAL_FAILURE=10

VERBOSE=0

RED='\033[0;41m'  #0;31m for text
GREEN='\033[0;42m'  #0;32 for text
NC='\033[0m' # No Color

FILE_OUTPUT="/dev/null"

#fonctions used by the tests

#vérifie que la fonction a bien renvoyée un EXIT_SUCCESS
#si c'est le cas, incrémente le compteur de succès
#sinon, affiche le message d'erreur et incrémente le compteur d'échec
test_exit_success(){
	echo -n "Result : "
	if [[ $EXIT_SUCCESS == $1 ]];
        then
                echo -e "${GREEN}*Passed* ${NC}\n" && SUCC=$((SUCC + 1)) && return
        fi
    echo -e "${NC}***Failed!***    ${NC}\n"
    cat $ERROR_FILE_TEST
    ERR=$((ERR + 1))
    return

}

test_exit_failure(){
	echo -n "Result : "
	if ((EXIT_FAILURE == $1)); then
	  test_stderr
	  if (($? == EXIT_SUCCESS)); then
	    SUCC=$((SUCC+1))
	    echo -e "${GREEN}*Passed*    ${NC}\n"
	    rm $ERROR_FILE_TEST
	    return
	  fi
	fi
	echo -e "${RED}***Failed!***    ${NC}\n"
	cat $ERROR_FILE_TEST
	ERR=$((ERR+1))
	return
}

#en théorie, test que le fichier erreur a bien été écrit,
#et qu'il est non vide
test_stderr(){
	if(test -f $ERROR_FILE_TEST); then
		[ -s $ERROR_FILE_TEST ] && return $EXIT_SUCCESS \
		|| echo "no error message found " && return $EXIT_FAILURE
	else
		echo "no error message found "
		return $EXIT_FAILURE
	fi
}

#verifie l'existence d'un fichier. Non utilisée.
test_file(){
	if(test -f $1); then
		echo "file $1 exist !"
	fi
}

#verifie le retour de valgrind. Sert à partir tu Homework 2
test_valgrind(){
	echo -n "Result : "
	if (($1 == $VAL_FAILURE))
	then
		echo -e "${RED}***Failed*** : memory problems detected  ${NC}\n"
		ERR=$((ERR+1))
	else
		echo -e "${GREEN}*Passed* : no memory problem  ${NC}\n"
		SUCC=$((SUCC+1))
	fi
}

#test la présence des fichiers de compilation
test_file_hierarchy(){
	for f in Makefile src/Makefile src/sudoku.c src/sudoku.h;
	do
		if ( test -f ${f} ); then
			echo -e "File ${f} is here\n"
			continue
		else 
			echo -e "missing file ${f}\n"
			return $EXIT_FAILURE
		fi
	done
	return $EXIT_SUCCESS
}

test_file_clean(){
	for f in sudoku src/sudoku src/sudoku.o;
	do
		if ( test -f ${f} ); then
			echo -e "File ${f} is here, it should not\n"
			return $EXIT_FAILURE
			continue
		else 
			continue
		fi
	done
	return $EXIT_SUCCESS
}


#debut des tests

start_time="$(date -u +%s.%N)"

echo "moving to ${PROJECT_PATH}"
#echo -e permet d'utilser le \n dans echo
echo -e '---------- ( File Hierarchy ) ----------\n'

cd ..;

test_file_hierarchy
test_exit_success $?

echo -e '---------- ( Build System ) ----------\n'

echo "* Check 'make clean'"
echo "Expected result: EXIT_SUCCESS"
make clean > /dev/null
test_exit_success $?

echo "* Check 'make'"
echo "Expected result: EXIT_SUCCESS"
make &> /dev/null
test_exit_success $?

#TODO
#Check requested files
#Check gcc call

echo "* Check 'make clean'"
echo "Expected result: clean target must remove all built files"
make clean > /dev/null
#TODO:
#check all built files were removed
test_exit_success $?

test_file_clean
test_exit_success $?

echo "* Check 'make help'"
echo "Expected result: EXIT_SUCCESS"
make help > /dev/null
test_exit_success $?

echo -e '---------- ( Option Parser ) ----------\n'

make &> /dev/null

echo "* Check './sudoku'"
echo "Expected result: EXIT_FAILURE and stderr is written"
./sudoku 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_failure $?


echo "* Check './sudoku -x'"
echo "Expected result: EXIT_FAILURE and stderr is written"
./sudoku -x 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_failure $?

echo "* Check './sudoku --xenophon'"
echo "Expected result: EXIT_FAILURE and stderr is written"
./sudoku --xenophon 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_failure $?

echo "* Check './sudoku -v'"
echo "Expected result: EXIT_FAILURE and stderr is written"
./sudoku -v 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_failure $?

echo "* Check './sudoku --verbose'"
echo "Expected result: EXIT_FAILURE and stderr is written"
./sudoku --verbose 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_failure $?


echo "* Check './sudoku -V'"
echo "Expected result: EXIT_SUCCESS"
./sudoku -V 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku --version'"
echo "Expected result: EXIT_SUCCESS"
./sudoku --version 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku -h'"
echo "Expected result: EXIT_SUCCESS"
./sudoku -h 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku --help'"
echo "Expected result: EXIT_SUCCESS"
./sudoku --help 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku --he'"
echo "Expected result: EXIT_SUCCESS"
./sudoku --he 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku grid-01-size_1-pass.sku'"
echo "Expected result: EXIT_SUCCESS"
./sudoku $TEST_PATH/grid-01-size_1-pass.sku 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku grid-01-size_1-pass.sku grid-02-size_4-pass.sku grid-03-size_9-pass.sku grid-04-size_16-pass.sku'"
echo "Expected result: EXIT_SUCCESS"
./sudoku $TEST_PATH/grid-01-size_1-pass.sku $TEST_PATH/grid-02-size_4-pass.sku $TEST_PATH/grid-03-size_9-pass.sku $TEST_PATH/grid-04-size_16-pass.sku 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku -v grid-01-size_1-pass.sku grid-02-size_4-pass.sku grid-03-size_9-pass.sku grid-04-size_16-pass.sku'"
echo "Expected result: EXIT_SUCCESS"
./sudoku -v $TEST_PATH/grid-01-size_1-pass.sku $TEST_PATH/grid-02-size_4-pass.sku $TEST_PATH/grid-03-size_9-pass.sku $TEST_PATH/grid-04-size_16-pass.sku 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku -x grid-01-size_1-pass.sku'"
echo "Expected result: EXIT_FAILURE and stderr is written"
./sudoku -x $TEST_PATH/grid-01-size_1-pass.sku 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_failure $?

echo "* Check './sudoku -v grid-01-size_1-pass.sku'"
echo "Expected result: EXIT_SUCCESS"
./sudoku -v $TEST_PATH/grid-01-size_1-pass.sku 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku --verbose grid-01-size_1-pass.sku'"
echo "Expected result: EXIT_SUCCESS"
./sudoku --verbose $TEST_PATH/grid-01-size_1-pass.sku 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku -o'"
echo "Expected result: EXIT_FAILURE and stderr is written"
./sudoku -o 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_failure $?

echo "* Check './sudoku --output'"
echo "Expected result: EXIT_FAILURE and stderr is written"
./sudoku --output 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_failure $?

echo "* Check './sudoku -o test.txt'"
echo "Expected result: EXIT_FAILURE and stderr is written"
./sudoku -o $PREVIOUS_PWD/test.txt 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_failure $?

echo "* Check './sudoku --output test.txt'"
echo "Expected result: EXIT_FAILURE and stderr is written"
./sudoku --output $PREVIOUS_PWD/test.txt 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_failure $?

echo "* Check './sudoku -o test.txt grid-01-size_1-pass.sku'"
echo "Expected result: EXIT_SUCCESS"
./sudoku -o $PREVIOUS_PWD/test.txt $TEST_PATH/grid-01-size_1-pass.sku 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku --output test.txt grid-01-size_1-pass.sku'"
echo "Expected result: EXIT_SUCCESS"
./sudoku --output $PREVIOUS_PWD/test.txt $TEST_PATH/grid-01-size_1-pass.sku 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?


echo "* Check './sudoku -o /test.txt grid-01-size_1-pass.sku'"
echo "Expected result: EXIT_FAILURE and stderr is written"
./sudoku -o /test.txt $TEST_PATH/grid-01-size_1-pass.sku 1> /dev/null 2> $ERROR_FILE_TEST
#test if file was created ?
test_exit_failure $?

echo "* Check './sudoku --output /test.txt grid-01-size_1-pass.sku'"
echo "Expected result: EXIT_FAILURE and stderr is written"
./sudoku --output /test.txt $TEST_PATH/grid-01-size_1-pass.sku 1> /dev/null 2> $ERROR_FILE_TEST
#test if file was create ?
test_exit_failure $?


echo "* Check './sudoku -g'"
echo "Expected result: EXIT_SUCCESS"
./sudoku -g 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku -g'"
echo "Expected result: EXIT_SUCCESS"
./sudoku -g 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku --generate'"
echo "Expected result: EXIT_SUCCESS"
./sudoku --generate 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku -g1'"
echo "Expected result: EXIT_SUCCESS"
./sudoku -g1 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku --generate=1'"
echo "Expected result: EXIT_SUCCESS"
./sudoku --generate=1 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku -g4'"
echo "Expected result: EXIT_SUCCESS"
./sudoku -g4 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku --generate=4'"
echo "Expected result: EXIT_SUCCESS"
./sudoku --generate=4 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku -g9'"
echo "Expected result: EXIT_SUCCESS"
./sudoku -g9 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku -g16'"
echo "Expected result: EXIT_SUCCESS"
./sudoku -g16 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku --generate=16'"
echo "Expected result: EXIT_SUCCESS"
./sudoku --generate=16 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku -g25'"
echo "Expected result: EXIT_SUCCESS"
./sudoku -g25 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku --generate=25'"
echo "Expected result: EXIT_SUCCESS"
./sudoku --generate=25 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku -g36'"
echo "Expected result: EXIT_SUCCESS"
./sudoku -g36 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku --generate=36'"
echo "Expected result: EXIT_SUCCESS"
./sudoku --generate=36 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku -g49'"
echo "Expected result: EXIT_SUCCESS"
./sudoku -g49 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku --generate=49'"
echo "Expected result: EXIT_SUCCESS"
./sudoku --generate=49 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku -g64'"
echo "Expected result: EXIT_SUCCESS"
./sudoku -g49 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku --generate=64'"
echo "Expected result: EXIT_SUCCESS"
./sudoku --generate=64 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku -g10'"
echo "Expected result: EXIT_FAILURE and stderr is written"
./sudoku -g10 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_failure $?

echo "* Check './sudoku --generate=10'"
echo "Expected result: EXIT_FAILURE and stderr is written"
./sudoku --generate=10 1> /dev/null 2> $ERROR_FILE_TEST
test_exit_failure $?


echo -e "---- Begin test for Homework 2 ----\n" 
echo -e "----------( Grid Parser )----------\n"

#pour afficher le retour de ./sudoku, commentez après le '1>' dans la première
#boucle

for f in ${TEST_PATH}* ; do
	if [[ "$f"  == *pass.sku ]]; then
		echo " Check ${f}"
		echo "Expected result: EXIT_SUCCESS"
		./sudoku $f 1> /dev/null 2> $ERROR_FILE_TEST 
		test_exit_success $?
	fi

	if [[ "$f" == *fail.sku ]]; then
	echo " Check ${f}"
		echo "Expected result: EXIT_FAILURE"
		./sudoku $f 1> /dev/null 2> $ERROR_FILE_TEST 
		test_exit_failure $?
	fi

done


echo "* Check './sudoku grid-02-size_4.sku  grid-03-size_9.sku'"
echo "Expected result: EXIT_SUCCESS"
./sudoku $TEST_PATH/grid-02-size_4-pass.sku $TEST_PATH/grid-03-size_9-pass.sku \
         1> /dev/null 2> $ERROR_FILE_TEST
test_exit_success $?

echo "* Check './sudoku  grid-02-size_4.sku  grid-09-unknown_char_l9-fail.sku'"
echo "Expected result: EXIT_FAILURE and stderr is written"
./sudoku  $TEST_PATH/grid-02-size_4-pass.sku $TEST_PATH/grid-09-unknown_char_l9-fail.sku \
          1> /dev/null 2> $ERROR_FILE_TEST
test_exit_failure $?

echo "* Check './sudoku  grid-09-unknown_char_l9-fail.sku  grid-02-size_4-pass.sku '"
echo "Expected result: EXIT_FAILURE and stderr is written"
./sudoku  $TEST_PATH/grid-09-unknown_char_l9-fail.sku $TEST_PATH/grid-02-size_4-pass.sku \
          1> /dev/null 2> $ERROR_FILE_TEST
test_exit_failure $?


echo -e "-------- ( Checking memory ) --------\n"
#mettre '\' permet de séparer les argument d'un longue commande sur 
#plusieurs lignes; 1> sortie standard; 2> messages d'erreurs

echo "* Check 'valgrind ./sudoku -h'"
valgrind --error-exitcode=$VAL_FAILURE --leak-check=full \
--log-file=$PREVIOUS_PWD/log.txt ./sudoku -h 1> /dev/null 2> $ERROR_FILE_TEST
test_valgrind $?
cat $PREVIOUS_PWD/log.txt
echo -e "\n"

#les tests valgrind sont les plus long à faire.
#le prof ne test normalement pas de grille "fail"

for f in ${TEST_PATH}* ; do
	if [[ "$f"  == *pass.sku ]]; then
		echo " Valgrind check ${f}"
		echo "Expected result: EXIT_SUCCESS"
		valgrind --error-exitcode=$VAL_FAILURE --leak-check=full \
                 --log-file=$PREVIOUS_PWD/log.txt  --errors-for-leak-kinds=all \
                 ./sudoku $f 1> /dev/null 2> $ERROR_FILE_TEST 
		test_valgrind $?
		cat $PREVIOUS_PWD/log.txt
		echo -e "\n"
	fi
done

echo "* Check 'valgrind ./sudoku grid-16x16-06-twice_char_8_in_line-inconsistent.sku'"
valgrind --error-exitcode=$VAL_FAILURE --leak-check=full \
--log-file=$PREVIOUS_PWD/log.txt --errors-for-leak-kinds=all \
./sudoku $TEST_PATH/grid-03-size_9-pass.sku $TEST_PATH/grid-05-size_25-pass.sku\
1> /dev/null 2> $ERROR_FILE_TEST
test_valgrind $?
cat $PREVIOUS_PWD/log.txt
echo -e "\n"



end_time="$(date -u +%s.%N)"
elapsed="$(bc <<<"$end_time-$start_time")"

echo -e "------- ( Final result ) --------\n"
echo "Passed: $SUCC ;Failed: $ERR"
echo "Total of $elapsed seconds elapsed for process"
cd $PREVIOUS_PWD

