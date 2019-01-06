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

#include <libvapory/VaporyPeer.h>
#include <libp2p/Host.h>
#include <test/tools/libtestvap/TestHelper.h>

using namespace std;
using namespace dev;
using namespace dev::vap;
using namespace dev::p2p;
using namespace dev::test;

class MockHostCapability: public p2p::HostCapabilityFace
{
protected:
	string name() const override { return "mock capability name"; }
	u256 version() const override { return 0; }
	unsigned messageCount() const override { return 0;  }
	std::shared_ptr<Capability> newPeerCapability(std::shared_ptr<SessionFace> const&, unsigned, CapDesc const&, uint16_t) override { return std::shared_ptr<Capability>(); }
};

class MockSession: public SessionFace
{
public:
	void start() override { }
	void disconnect(DisconnectReason /*_reason*/) override { }

	void ping() override { }

	bool isConnected() const override { return true; }

	NodeID id() const override { return {}; }

	void sealAndSend(RLPStream& _s, uint16_t /*_protocolID*/) override
	{
		_s.swapOut(m_bytesSent);
	}

	int rating() const override { return 0; }
	void addRating(int /*_r*/) override { }

	void addNote(string const& _k, string const& _v) override
	{
		m_notes[_k] = _v;
	}

	PeerSessionInfo info() const override { return PeerSessionInfo{ NodeID{}, "", "", 0, std::chrono::steady_clock::duration{}, {}, 0, {}, 0 }; }
	std::chrono::steady_clock::time_point connectionTime() override { return std::chrono::steady_clock::time_point{}; }

	void registerCapability(CapDesc const& /*_desc*/, std::shared_ptr<Capability> /*_p*/) override { }
	void registerFraming(uint16_t /*_id*/) override { }

	map<CapDesc, std::shared_ptr<Capability>> const&  capabilities() const override { return m_capabilities; }

	std::shared_ptr<Peer> peer() const override { return nullptr;  }

	std::chrono::steady_clock::time_point lastReceived() const override { return std::chrono::steady_clock::time_point{}; }

	ReputationManager& repMan() override { return m_repMan; }

	ReputationManager m_repMan;
	map<CapDesc, std::shared_ptr<Capability>> m_capabilities;
	bytes m_bytesSent;
	map<string, string> m_notes;
};


class MockVaporyPeerObserver: public VaporyPeerObserverFace
{
public:
	void onPeerStatus(std::shared_ptr<VaporyPeer>) override {}

	void onPeerTransactions(std::shared_ptr<VaporyPeer>, RLP const&) override {}

	void onPeerBlockHeaders(std::shared_ptr<VaporyPeer>, RLP const&) override {}

	void onPeerBlockBodies(std::shared_ptr<VaporyPeer>, RLP const&) override {}

	void onPeerNewHashes(std::shared_ptr<VaporyPeer>, std::vector<std::pair<h256, u256>> const&) override {}

	void onPeerNewBlock(std::shared_ptr<VaporyPeer>, RLP const&) override {}

	void onPeerNodeData(std::shared_ptr<VaporyPeer>, RLP const&) override {}

	void onPeerReceipts(std::shared_ptr<VaporyPeer>, RLP const&) override {}

	void onPeerAborting() override {}
};

class VaporyPeerTestFixture: public TestOutputHelper
{
public:
	VaporyPeerTestFixture():
		session(std::make_shared<MockSession>()),
		observer(std::make_shared<MockVaporyPeerObserver>()),
		offset(UserPacket),
		peer(session, &hostCap, offset, { "vap", 0 }, 0)
	{
		peer.init(63, 2, 0, h256(0), h256(0), std::shared_ptr<VaporyHostDataFace>(), observer);
	}

	MockHostCapability hostCap;
	std::shared_ptr<MockSession> session;
	std::shared_ptr<MockVaporyPeerObserver> observer;
	uint8_t offset;
	VaporyPeer peer;
};

BOOST_FIXTURE_TEST_SUITE(VaporyPeerSuite, VaporyPeerTestFixture)

BOOST_AUTO_TEST_CASE(VaporyPeerSuite_requestNodeData)
{
	h256 dataHash("0x949d991d685738352398dff73219ab19c62c06e6f8ce899fbae755d5127ed1ef");
	peer.requestNodeData({ dataHash });

	uint8_t code = static_cast<uint8_t>(session->m_bytesSent[0]);
	BOOST_REQUIRE_EQUAL(code, offset + 0x0d);

	bytes payloadSent(session->m_bytesSent.begin() + 1, session->m_bytesSent.end());
	RLP rlp(payloadSent);
	BOOST_REQUIRE(rlp.isList());
	BOOST_REQUIRE_EQUAL(rlp.itemCount(), 1);
	BOOST_REQUIRE_EQUAL(static_cast<h256>(rlp[0]), dataHash);
}

BOOST_AUTO_TEST_CASE(VaporyPeerSuite_requestNodeDataSeveralHashes)
{
	h256 dataHash0("0x949d991d685738352398dff73219ab19c62c06e6f8ce899fbae755d5127ed1ef");
	h256 dataHash1("0x0e4562a10381dec21b205ed72637e6b1b523bdd0e4d4d50af5cd23dd4500a217");
	h256 dataHash2("0x53ab44f45948543775a4c405085b918e5e648db1201283bb54a59701afdaedf3");
	peer.requestNodeData({ dataHash0, dataHash1, dataHash2 });

	bytes payloadSent(session->m_bytesSent.begin() + 1, session->m_bytesSent.end());
	RLP rlp(payloadSent);
	BOOST_REQUIRE_EQUAL(rlp.itemCount(), 3);
	BOOST_REQUIRE_EQUAL(static_cast<h256>(rlp[0]), dataHash0);
	BOOST_REQUIRE_EQUAL(static_cast<h256>(rlp[1]), dataHash1);
	BOOST_REQUIRE_EQUAL(static_cast<h256>(rlp[2]), dataHash2);
}

BOOST_AUTO_TEST_CASE(VaporyPeerSuite_requestNodeDataAddsAskingNote)
{
	h256 dataHash("0x949d991d685738352398dff73219ab19c62c06e6f8ce899fbae755d5127ed1ef");
	peer.requestNodeData({ h256("0x949d991d685738352398dff73219ab19c62c06e6f8ce899fbae755d5127ed1ef") });

	BOOST_REQUIRE(session->m_notes.find("ask") != session->m_notes.end());
	BOOST_REQUIRE(session->m_notes["ask"] == "NodeData");
}

BOOST_AUTO_TEST_CASE(VaporyPeerSuite_requestNodeDataWithNoHashesSetsAskNoteToNothing)
{
	peer.requestNodeData({});

	BOOST_REQUIRE(session->m_notes.find("ask") != session->m_notes.end());
	BOOST_REQUIRE(session->m_notes["ask"] == "Nothing");
}

BOOST_AUTO_TEST_CASE(VaporyPeerSuite_requestReceipts)
{
	h256 blockHash0("0x949d991d685738352398dff73219ab19c62c06e6f8ce899fbae755d5127ed1ef");
	h256 blockHash1("0x0e4562a10381dec21b205ed72637e6b1b523bdd0e4d4d50af5cd23dd4500a217");
	h256 blockHash2("0x53ab44f45948543775a4c405085b918e5e648db1201283bb54a59701afdaedf3");
	peer.requestReceipts({ blockHash0, blockHash1, blockHash2 });

	bytes payloadSent(session->m_bytesSent.begin() + 1, session->m_bytesSent.end());
	RLP rlp(payloadSent);
	BOOST_REQUIRE_EQUAL(rlp.itemCount(), 3);
	BOOST_REQUIRE_EQUAL(static_cast<h256>(rlp[0]), blockHash0);
	BOOST_REQUIRE_EQUAL(static_cast<h256>(rlp[1]), blockHash1);
	BOOST_REQUIRE_EQUAL(static_cast<h256>(rlp[2]), blockHash2);
}

BOOST_AUTO_TEST_SUITE_END()
