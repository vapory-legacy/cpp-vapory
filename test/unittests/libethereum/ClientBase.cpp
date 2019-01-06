/*
	This file is part of cpp-vapory.

	cpp-vapory is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	cpp-vapory is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with cpp-vapory.  If not, see <http://www.gnu.org/licenses/>.
 */
/** @file ClientBase.cpp
 * @author Marek Kotewicz <marek@ethdev.com>
 * @date 2015
 */

#include <boost/test/unit_test.hpp>
#include <libdevcore/CommonJS.h>
#include <libvapashseal/Vapash.h>
#include <test/tools/libtestvap/TestOutputHelper.h>
#include <test/tools/libtestvap/TestUtils.h>
#include <test/tools/libtestutils/FixedClient.h>

using namespace std;
using namespace dev;
using namespace dev::vap;
using namespace dev::test;

BOOST_FIXTURE_TEST_SUITE(ClientBase, ParallelClientBaseFixture)

BOOST_AUTO_TEST_CASE(blocks)
{
	TestOutputHelper testOutputHelper{};
	enumerateClients([](Json::Value const& _json, dev::vap::ClientBase& _client) -> void
	{
		auto compareState = [&_client](Json::Value const& _o, string const& _name, BlockNumber _blockNumber) -> void
		{
			Address address(_name);

			// balanceAt
			u256 expectedBalance = u256(_o["balance"].asString());
			u256 balance = _client.balanceAt(address, _blockNumber);
			VAP_CHECK_EQUAL(expectedBalance, balance);

			// countAt
			u256 expectedCount = u256(_o["nonce"].asString());
			u256 count = _client.countAt(address,  _blockNumber);
			VAP_CHECK_EQUAL(expectedCount, count);

			// stateAt
			for (string const& pos: _o["storage"].getMemberNames())
			{
				u256 expectedState = u256(_o["storage"][pos].asString());
				u256 state = _client.stateAt(address, u256(pos), _blockNumber);
				VAP_CHECK_EQUAL(expectedState, state);
			}

			// codeAt
			bytes expectedCode = fromHex(_o["code"].asString());
			bytes code = _client.codeAt(address, _blockNumber);
			VAP_CHECK_EQUAL_COLLECTIONS(expectedCode.begin(), expectedCode.end(),
										code.begin(), code.end());
		};

		for (string const& name: _json["postState"].getMemberNames())
		{
			Json::Value o = _json["postState"][name];
			compareState(o, name, PendingBlock);
		}

		for (string const& name: _json["pre"].getMemberNames())
		{
			Json::Value o = _json["pre"][name];
			compareState(o, name, 0);
		}

		// number
		unsigned expectedNumber = _json["blocks"].size();
		unsigned number = _client.number();
		VAP_CHECK_EQUAL(expectedNumber, number);
		
		u256 totalDifficulty = u256(_json["genesisBlockHeader"]["difficulty"].asString());
		for (Json::Value const& block: _json["blocks"])
		{
			Json::Value blockHeader = block["blockHeader"];
			Json::Value uncles = block["uncleHeaders"];
			Json::Value transactions = block["transactions"];
			h256 blockHash = h256(fromHex(blockHeader["hash"].asString()));
			
			// just update the difficulty
			for (Json::Value const& uncle: uncles)
			{
				totalDifficulty += u256(uncle["difficulty"].asString());
			}
			
			// hashFromNumber
			h256 expectedHashFromNumber = h256(fromHex(blockHeader["hash"].asString()));
			h256 hashFromNumber = _client.hashFromNumber(jsToInt(blockHeader["number"].asString()));
			VAP_CHECK_EQUAL(expectedHashFromNumber, hashFromNumber);
			
			// blockInfo
			auto compareBlockInfos = [](Json::Value const& _b, BlockHeader _blockInfo) -> void
			{
				LogBloom expectedBlockInfoBloom = LogBloom(fromHex(_b["bloom"].asString()));
				Address expectedBlockInfoCoinbase = Address(fromHex(_b["coinbase"].asString()));
				u256 expectedBlockInfoDifficulty = u256(_b["difficulty"].asString());
				bytes expectedBlockInfoExtraData = fromHex(_b["extraData"].asString());
				u256 expectedBlockInfoGasLimit = u256(_b["gasLimit"].asString());
				u256 expectedBlockInfoGasUsed = u256(_b["gasUsed"].asString());
				h256 expectedBlockInfoHash = h256(fromHex(_b["hash"].asString()));
				h256 expectedBlockInfoMixHash = h256(fromHex(_b["mixHash"].asString()));
				vap::Nonce expectedBlockInfoNonce = vap::Nonce(fromHex(_b["nonce"].asString()));
				u256 expectedBlockInfoNumber = u256(_b["number"].asString());
				h256 expectedBlockInfoParentHash = h256(fromHex(_b["parentHash"].asString()));
				h256 expectedBlockInfoReceiptsRoot = h256(fromHex(_b["receiptTrie"].asString()));
				u256 expectedBlockInfoTimestamp = u256(_b["timestamp"].asString());
				h256 expectedBlockInfoTransactionsRoot = h256(fromHex(_b["transactionsTrie"].asString()));
				h256 expectedBlockInfoUncldeHash = h256(fromHex(_b["uncleHash"].asString()));
				VAP_CHECK_EQUAL(expectedBlockInfoBloom, _blockInfo.logBloom());
				VAP_CHECK_EQUAL(expectedBlockInfoCoinbase, _blockInfo.author());
				VAP_CHECK_EQUAL(expectedBlockInfoDifficulty, _blockInfo.difficulty());
				VAP_CHECK_EQUAL_COLLECTIONS(
					expectedBlockInfoExtraData.begin(),
					expectedBlockInfoExtraData.end(),
					_blockInfo.extraData().begin(),
					_blockInfo.extraData().end()
				);
				VAP_CHECK_EQUAL(expectedBlockInfoGasLimit, _blockInfo.gasLimit());
				VAP_CHECK_EQUAL(expectedBlockInfoGasUsed, _blockInfo.gasUsed());
				VAP_CHECK_EQUAL(expectedBlockInfoHash, _blockInfo.hash());
				VAP_CHECK_EQUAL(expectedBlockInfoMixHash, Vapash::mixHash(_blockInfo));
				VAP_CHECK_EQUAL(expectedBlockInfoNonce, Vapash::nonce(_blockInfo));
				VAP_CHECK_EQUAL(expectedBlockInfoNumber, _blockInfo.number());
				VAP_CHECK_EQUAL(expectedBlockInfoParentHash, _blockInfo.parentHash());
				VAP_CHECK_EQUAL(expectedBlockInfoReceiptsRoot, _blockInfo.receiptsRoot());
				VAP_CHECK_EQUAL(expectedBlockInfoTimestamp, _blockInfo.timestamp());
				VAP_CHECK_EQUAL(expectedBlockInfoTransactionsRoot, _blockInfo.transactionsRoot());
				VAP_CHECK_EQUAL(expectedBlockInfoUncldeHash, _blockInfo.sha3Uncles());
			};

			BlockHeader blockInfo((static_cast<FixedClient&>(_client)).bc().headerData(blockHash), HeaderData);
			compareBlockInfos(blockHeader, blockInfo);

			// blockDetails
			unsigned expectedBlockDetailsNumber = jsToInt(blockHeader["number"].asString());
			totalDifficulty += u256(blockHeader["difficulty"].asString());
			BlockDetails blockDetails = _client.blockDetails(blockHash);
			VAP_CHECK_EQUAL(expectedBlockDetailsNumber, blockDetails.number);
			VAP_CHECK_EQUAL(totalDifficulty, blockDetails.totalDifficulty);
			
			auto compareTransactions = [](Json::Value const& _t, Transaction _transaction) -> void
			{
				bytes expectedTransactionData = fromHex(_t["data"].asString());
				u256 expectedTransactionGasLimit = u256(_t["gasLimit"].asString());
				u256 expectedTransactionGasPrice = u256(_t["gasPrice"].asString());
				u256 expectedTransactionNonce = u256(_t["nonce"].asString());
				u256 expectedTransactionSignatureR = h256(fromHex(_t["r"].asString()));
				u256 expectedTransactionSignatureS = h256(fromHex(_t["s"].asString()));
//				unsigned expectedTransactionSignatureV = jsToInt(t["v"].asString());
				
				VAP_CHECK_EQUAL_COLLECTIONS(
					expectedTransactionData.begin(),
					expectedTransactionData.end(),
					_transaction.data().begin(),
					_transaction.data().end()
				);
				VAP_CHECK_EQUAL(expectedTransactionGasLimit, _transaction.gas());
				VAP_CHECK_EQUAL(expectedTransactionGasPrice, _transaction.gasPrice());
				VAP_CHECK_EQUAL(expectedTransactionNonce, _transaction.nonce());
				VAP_CHECK_EQUAL(expectedTransactionSignatureR, _transaction.signature().r);
				VAP_CHECK_EQUAL(expectedTransactionSignatureS, _transaction.signature().s);
//				VAP_CHECK_EQUAL(expectedTransactionSignatureV, _transaction.signature().v); // 27 === 0x0, 28 === 0x1, not sure why
			};

			Transactions ts = _client.transactions(blockHash);
			TransactionHashes tHashes = _client.transactionHashes(blockHash);
			unsigned tsCount = _client.transactionCount(blockHash);
			
			VAP_REQUIRE(transactions.size() == ts.size());
			VAP_REQUIRE(transactions.size() == tHashes.size());
			
			// transactionCount
			VAP_CHECK_EQUAL(transactions.size(), tsCount);
			
			for (unsigned i = 0; i < tsCount; i++)
			{
				Json::Value t = transactions[i];
				
				// transaction (by block hash and transaction index)
				Transaction transaction = _client.transaction(blockHash, i);
				compareTransactions(t, transaction);

				// transaction (by hash)
				Transaction transactionByHash = _client.transaction(transaction.sha3());
				compareTransactions(t, transactionByHash);
				
				// transactions
				compareTransactions(t, ts[i]);
				
				// transactionHashes
				VAP_CHECK_EQUAL(transaction.sha3(), tHashes[i]);
			}
			
			// uncleCount
			unsigned usCount = _client.uncleCount(blockHash);
			VAP_CHECK_EQUAL(uncles.size(), usCount);
			
			for (unsigned i = 0; i < usCount; i++)
			{
				Json::Value u = uncles[i];
				
				// uncle (by hash)
				BlockHeader uncle = _client.uncle(blockHash, i);
				compareBlockInfos(u, uncle);
			}
		}
	});
}

BOOST_AUTO_TEST_SUITE_END()
