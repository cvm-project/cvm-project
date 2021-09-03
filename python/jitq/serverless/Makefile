SCRIPTPATH:=$(dir $(realpath $(lastword $(MAKEFILE_LIST))))
PROJECTPATH:=$(realpath $(SCRIPTPATH)/../../..)

DOCKERIMAGE=ingomuellernet/lambda-build-jitq:2021-09-03

all: build dependencies aws_lambda/lib
	docker run --rm -it \
		-v $(PROJECTPATH)/python/jitq/serverless/dependencies:/output/opt:rw \
		-v $(PROJECTPATH)/python/jitq/serverless/aws_lambda/lib:/output/lib:rw \
		-v $(PROJECTPATH)/python/jitq/serverless/build:/output/build:rw \
		-v $(PROJECTPATH):/jitq \
		$(DOCKERIMAGE) /jitq/tools/aws/build-lambda-env.sh

build dependencies aws_lambda/lib:
	mkdir $@
	chmod ugo+wx $@

clean:
	rm -rf build dependencies aws_lambda/lib
