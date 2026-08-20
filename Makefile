.PHONY: test lint format cpp node secrets verify

PYTHON ?= python3
CXX ?= c++

lint:
	ruff check .

format:
	ruff format --check .

test:
	pytest -q

node:
	node decoder/decoder.test.js

cpp:
	$(CXX) -std=c++17 -Ifirmware/nfp_environment_node tests/cpp/test_payload.cpp -o /tmp/nfp-payload-test
	/tmp/nfp-payload-test

secrets:
	$(PYTHON) scripts/check_no_secrets.py

verify: lint format test node cpp secrets
	$(PYTHON) -m tools.summarize_evaluation > /tmp/nfp-evaluation.md
	@echo "All Nature and Facilities Protector checks passed."
